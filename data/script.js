// IDs for the websocket communication
const ID_REQUEST_MANUAL_FROM_SERVER = 0;
const ID_SEND_MANUAL_TO_CLIENT = 1;
const ID_UPDATE_MANUAL = 2;

const ID_REQUEST_SCHEDULE_FROM_SERVER = 10;
const ID_SEND_SCHEDULE_TO_CLIENT = 11;
const ID_SAVE_SCHEDULE = 12;

const ID_REQUEST_SETTINGS_FROM_SERVER = 20;
const ID_SEND_SETTINGS_TO_CLIENT = 21;
const ID_SAVE_SETTINGS = 22;

const ID_RESTART = 50;
const ID_FACTORY_SETTINGS = 51;

// PWM Generators
const PWM_GENERATOR_ESP8266 = 0;
const PWM_GENERATOR_PCA9685 = 1;

// name definitions for the JSON Format
const CHAR_NUM_OF_CHANNELS = "numOfChannels";
const CHAR_MAX_NUM_OF_CHANNELS = "maxNumOfChannels";
const CHAR_MAX_NUM_OF_ENTRIES = "maxNumOfEntries";

const CHAR_PWM_FREQUENCY = "PWMFrequency";
const CHAR_PWM_GENERATOR = "PWMGenerator";

const CHAR_NTP_SERVER = "NTPServer";
const CHAR_TIMEZONE = "timezone";
const CHAR_TIME = "time";

const CHAR_CURRENT_POWER = "currentPower";

const CHAR_CHANNELS = "channels";
const CHAR_CHANNEL_NAME = "name";
const CHAR_CHANNEL_COLOR = "color";
const CHAR_CHANNEL_MANUAL = "manual";
const CHAR_CHANNEL_MOONLIGHT = "moonlight";
const CHAR_CHANNEL_MAX_MOONLIGHT_VALUE = "MaxMoonlightValue";
const CHAR_CHANNEL_PIN = "pin";
const CHAR_CHANNEL_POWER = "power";
const CHAR_CHANNEL_VALUE = "value";
const CHAR_CHANNEL_TIMES = "times";
const CHAR_CHANNEL_VALUES = "values";

// global variables
var websocket = new WebSocket('ws://' + location.hostname + ':81');
var json; // incoming json from server
var chart; // chart for the schedule page

/*
 * Websocket interaction
*/
// opening event
websocket.onopen = function (openEvent) {
  console.log("websocket OPEN: " + JSON.stringify(openEvent, null, 4));
};
// error event
websocket.onerror = function (errorEvent) {
  console.log("websocket ERROR: " + JSON.stringify(errorEvent, null, 4));
};
// receiving message
websocket.onmessage = function (messageEvent) {
  var wsMsg = messageEvent.data;
  console.log("websocket RECEIVE MESSAGE: " + wsMsg);
  json = JSON.parse(wsMsg);
  switch(json.id) {
    case ID_SEND_MANUAL_TO_CLIENT:
      displayManual();
      break;    
    case ID_SEND_SCHEDULE_TO_CLIENT:
      displaySchedule();
      break;
    case ID_SEND_SETTINGS_TO_CLIENT:
      displaySettings();
      break;
  }   
}
// sends a msg via the websocket
function sendWebsocketMsg(msg) {
  console.log("websocket SEND MESSAGE: "+msg);
  websocket.send(msg);
}


/* 
 * functions for the manual page
 */
// loads the manual page
function displayManual() {
  content = "";
  content = "<table class=\"indexTable\"><tr><th></th><th>Name</th><th>Manual</th><th>Value</th><th></th></tr>";
  for(c=0; c<json[CHAR_CHANNELS].length; c++) {
    channel = json[CHAR_CHANNELS][c];
    if(!channel[CHAR_CHANNEL_MOONLIGHT]) {
      // color
      content += "<tr><td bgcolor='"+channel[CHAR_CHANNEL_COLOR]+"' width='20'></td>";
      // name
      content += "<td>"+channel[CHAR_CHANNEL_NAME]+"</td>";
      // manual
      content += "<td><input id='manual_checkbox_"+c+"' onclick='updateManual(\"checkbox\", "+c+");' type='checkbox'";
        if(channel[CHAR_CHANNEL_MANUAL]) content += " checked";
        content += "></td>";
      // value
      content += "<td><input id='slider_"+c+"' onchange='updateManual(\"slider\", "+c+");' ";
        content += "value='"+Math.round(channel[CHAR_CHANNEL_VALUE]*Math.pow(10,2))/Math.pow(10,2)+"' type='range' min='0' max='100' step='0.5'></td>";
        content += "<td><span id='value_num_"+c+"'>"+Math.round(channel[CHAR_CHANNEL_VALUE]*Math.pow(10,2))/Math.pow(10,2)+"%</span></td></tr>";
    }
  }
  content  += "</table>";
  document.getElementById('content_div').innerHTML = content;  
}
// updates the changed values from the manual page to the server
function updateManual(type, c) {
  if(type=='checkbox') {
    json[CHAR_CHANNELS][c][CHAR_CHANNEL_MANUAL] = document.getElementById('manual_checkbox_'+c).checked;
  }
  if(type=='slider') {
    // set channel to manual
    document.getElementById('manual_checkbox_'+c).checked = true;
    json[CHAR_CHANNELS][c][CHAR_CHANNEL_MANUAL] = true;
    // update values
    json[CHAR_CHANNELS][c][CHAR_CHANNEL_VALUE] = document.getElementById('slider_'+c).value;
    document.getElementById('value_num_'+c).innerHTML = Math.round(document.getElementById('slider_'+c).value*Math.pow(10,2))/Math.pow(10,2)+"%";
  }
  json.id = ID_UPDATE_MANUAL;
  sendWebsocketMsg(JSON.stringify(json));
  if(type=='checkbox') {
    var tmp = {"id":ID_REQUEST_MANUAL_FROM_SERVER};
    sendWebsocketMsg(JSON.stringify(tmp));  
  }
}

/*
 * functions for the schedule page
 */
// loads the schedule page
const heightChart = "500";
function displaySchedule() {
  content = "<div id='chart_container' style='height: "+heightChart+"px;'></div>";
  content += "<button class='scheduleButton' onclick='deleteEntry();'>Delete Point</button>";
  content += "<button class='scheduleButton' onclick='addEntry();'>Add Point</button>";
  content += "<button class='scheduleButton' onclick='openContent(\"schedule\");'>Reload</button>";
  content += "<button class='scheduleButton' onclick='saveSchedule();'>Save</button>";
  document.getElementById('content_div').innerHTML = content;
  // creates the chart
  chart = new Highcharts.Chart({
    chart: {
      renderTo: 'chart_container',
      animation: false,
      plotBorderWidth: 1,
      height: heightChart,
      width: 1200,
    },
    title: {
      text: null
    },
      tooltip: {
        headerFormat: '<b>{series.name}</b><br>',
        xDateFormat: '%M:%S',
        pointFormat: 'time: {point.x:%H:%M:%S}<br>value: {point.y:.2f}%'
    },
    plotOptions: {
      series: {
        point: {
          events: {
            drag: function(e) {
              if (e.y > 100) {
                  this.y = 100;
                  return false;
              }
              if (e.y < 0) {
                  this.y = 0;
                  return false;
              }           
              if (e.x < Date.UTC(2000,0,0,0,0,0)) {
                this.x = Date.UTC(2000,0,0,0,0,0);
                return false;
              }
              if (e.x > Date.UTC(2000,0,0,23,59,59)) {
                this.x = Date.UTC(2000,0,0,23,59,59);
                return false;
              }            
            },
          }
        }
      }
    },
    xAxis: {
      type: 'datetime',
      max: Date.UTC(2000, 0, 0, 23, 59, 59),
      min: Date.UTC(2000, 0, 0, 0, 0, 1),
      gridLineWidth: 1,
      plotLines: [{
        color: '#FF0000', // Red
        width: 2,
        value: Date.UTC(2000,0,0,0,0,json[CHAR_TIME]) // Position, you'll have to translate this to the values on your x axis
    }]
    },
    yAxis: {
      max: 100,
      min: 0,
      gridLineWidth: 1,
      labels: {
        format: '{value}%'
      }
    },
  });

  // fill chart
  for(c=0;c<json[CHAR_CHANNELS].length;c++) {
    channel = json[CHAR_CHANNELS][c];
    if(!channel[CHAR_CHANNEL_MOONLIGHT]) {
      // prepare data constisting of entries (time-value pair)
      data = new Array();
      for(i=0;i<channel[CHAR_CHANNEL_TIMES].length;i++) {
        entry = new Array();
        entry.push(Date.UTC(2000,0,0,0,0,channel[CHAR_CHANNEL_TIMES][i]));
        entry.push(channel[CHAR_CHANNEL_VALUES][i]);
        data.push(entry);
      }
      // add series
      chart.addSeries({
          allowPointSelect: true,
          type: 'line',
          name: channel[CHAR_CHANNEL_NAME],
          color: channel[CHAR_CHANNEL_COLOR],
          cursor: 'move',
          marker: {
            enabled: true
          },
          draggableX: true,
          draggableY: true,
          data: data,
      });
    }
  }
}
// removes the selected entry
function deleteEntry() {
  p = chart.getSelectedPoints();
  if (p.length == 0) {
    window.alert("Select the point first you want to delete");
    return;
  }
  series_ = p[0].series._i;
  number_ = p[0].index;
  chart.series[series_].removePoint(number_)
}
// adds an entry to the selected series
function addEntry() {
  p = chart.getSelectedPoints();
  if (p.length == 0) {
    window.alert("Select a point of the channel you want to add a point");
    return;
  }
  if(p[0].series.data.length >= json[CHAR_MAX_NUM_OF_ENTRIES]) {
    window.alert("Maximal number of points for the channel reached");
    return;    
  }
  series_ = p[0].series._i; 
  chart.series[series_].addPoint([Date.UTC(2000, 0, 0, 23, 0,0), 10]);
}
// sends the schedule to the server to save it
function saveSchedule() {
  json_ = new Object();
  json_.id = ID_SAVE_SCHEDULE;
  json_[CHAR_CHANNELS] = new Array();
    for(c=0;c<chart.series.length;c++) {
    json_[CHAR_CHANNELS][c] = new Object();
    json_[CHAR_CHANNELS][c][CHAR_CHANNEL_TIMES] = new Array();
    json_[CHAR_CHANNELS][c][CHAR_CHANNEL_VALUES] = new Array();
    for(i=0;i<chart.series[c].data.length;i++) {
      v = chart.series[c].data[i].y;
      t_ = chart.series[c].data[i].x;
      t = new Date(t_);
      json_[CHAR_CHANNELS][c][CHAR_CHANNEL_VALUES].push(v);
      json_[CHAR_CHANNELS][c][CHAR_CHANNEL_TIMES].push( t.getUTCHours()*60*60 + t.getUTCMinutes()*60 + t.getUTCSeconds() );
    }
  }
  // send json
  sendWebsocketMsg(JSON.stringify(json_));
  openContent("schedule");
}


/*
 * functions for the settings page
 */
// loads the settings page
function displaySettings() {
  content = "";
  // form that detects changes
  content += "<form onchange='updateSettings();'>";
  
  // MainTable with settings
  content += "<table class='settingsMainTable'>";
  // Number of channels
  content += "<tr><th>Number of Channels</th>";
    content += "<td><select id='"+CHAR_NUM_OF_CHANNELS+"'>";
    for(c=1; c<=json[CHAR_MAX_NUM_OF_CHANNELS]; c++) {
      content += "<option value = '"+c+"' ";
      if(c==json[CHAR_NUM_OF_CHANNELS]) content += " selected ";
      content += ">"+c+"</option>";
    }
    content += "</select></td></tr>";
  // PWM generator
  content += "<tr><th>PWM Generator</th><td>";
    content += "<select id='"+CHAR_PWM_GENERATOR+"'>";
    content += "<option value='"+PWM_GENERATOR_ESP8266+"' ";
    if(json[CHAR_PWM_GENERATOR] == PWM_GENERATOR_ESP8266) content += "selected";
    content += ">ESP8266</option>";
    content += "<option value='"+PWM_GENERATOR_PCA9685+"' ";
    if(json[CHAR_PWM_GENERATOR] == PWM_GENERATOR_PCA9685) content += "selected";
    content += ">PCA9685</option>";
    content += "</select>";
    content += "</td></tr>";
  // PWM frequency
  content += "<tr><th>PWM Frequency [Hz]</th><td><input type='number' id='"+CHAR_PWM_FREQUENCY+"' value='"+json[CHAR_PWM_FREQUENCY]+"'</td></tr>";
  // current power
  content += "<tr><th>Current Power Consumption[W]</th><td>"+json[CHAR_CURRENT_POWER].toFixed(2)+"</td></tr>";
  // time
  tmp = new Date((json[CHAR_TIME]+60*60*json[CHAR_TIMEZONE])*1000);
  content += "<tr><th>Time</th><td>"+("0"+tmp.getUTCHours()).slice(-2)+":"+("0"+tmp.getUTCMinutes()).slice(-2)+":"+("0"+tmp.getUTCSeconds()).slice(-2)+"</td></tr>";  
  // timezone
  content += "<tr><th>Timezone</th><td><input id='"+CHAR_TIMEZONE+"'type='number' value='"+json[CHAR_TIMEZONE]+"'</td></tr>";    
  content += "</table><br><br>";

  // ChannelTable with channel settings
  content += "<table class='settingsChannelTable'><tr>";
  // Channel number
  content += "<th>Channel</th>";
  // Channel name
  content += "<th>Name</th>";
  // Channel color
  content += "<th>Color</th>";

  // Channel moonlight
  content += "<th>Moonlight</th>";
  // Channel moonlight value
  content += "<th>Max Moonlight [%]</th>";
  // Channel power
  content += "<th>Power [Watts]</th>";
  // Channel pin (only showed if PWM Signal is generated by the ESP8266 itself
  if(json[CHAR_PWM_GENERATOR] == PWM_GENERATOR_ESP8266) {
    content += "<th>PWM Pin on ESP8266</th>";
  }  
  content += "</tr>";
  for(c=0; c<json[CHAR_NUM_OF_CHANNELS]; c++) {
    channel = json[CHAR_CHANNELS][c];
    content += "<tr>";
    // Channel number
    content += "<td>"+(c+1)+"</td>";
    // Channel name
    content += "<td><input id='"+CHAR_CHANNEL_NAME+"_"+c+"' type='text' value='"+channel[CHAR_CHANNEL_NAME]+"' maxlength='20' size='16'></td>";
    // Channel color
    content += "<td><input id='"+CHAR_CHANNEL_COLOR+"_"+c+"' type='color' value='"+channel[CHAR_CHANNEL_COLOR]+"'></td>";
    // Channel moonlight
    content += "<td><input id='"+CHAR_CHANNEL_MOONLIGHT+"_"+c+"' type='checkbox'";
      if(channel[CHAR_CHANNEL_MOONLIGHT]) content += " checked ";
      content += "></td>";
    // Channel moonlight value
    content += "<td>";
    if(channel[CHAR_CHANNEL_MOONLIGHT]) {
      content += "<input type='number' id='"+CHAR_CHANNEL_MAX_MOONLIGHT_VALUE+"_"+c+"' value='"+channel[CHAR_CHANNEL_MAX_MOONLIGHT_VALUE]+"' min='0' max='101'>";
    }
    content += "</td>";
    // channel power
    content += "<td><input id='"+CHAR_CHANNEL_POWER+"_"+c+"' type='number' value='"+channel[CHAR_CHANNEL_POWER]+"' min='0'></td>";
    // Channel pin
    if(json[CHAR_PWM_GENERATOR] == PWM_GENERATOR_ESP8266) {
      content += "<td><input id='"+CHAR_CHANNEL_PIN+"_"+c+"' type='number' value='"+channel[CHAR_CHANNEL_PIN]+"'></td>";
    }
    content += "</tr>";
  }
  content += "</table>";
  
  // end form that detects changes
  content += "</form><br>";  
  // save
  content += "<button type='submit' onclick='saveSettings();'>Save</button>";
  // reload
  content += "<button type='submit' onclick='openContent(\"settings\");')>Reload</button>";
  // restart
  content += "<button type='submit' onclick='restart();'>Restart</button>";
  // restore factory settings
  content += "<button type='submit' onclick='factorySettings();'>Restore Factory Settings</button>";
  
  document.getElementById('content_div').innerHTML = content;
}
// updates the settings on the settings page
function updateSettings() {

  // channels
  for(c=0; c<json[CHAR_NUM_OF_CHANNELS]; c++) {
    // name
    json[CHAR_CHANNELS][c][CHAR_CHANNEL_NAME] = document.getElementById(CHAR_CHANNEL_NAME+"_"+c).value;
    // color
    json[CHAR_CHANNELS][c][CHAR_CHANNEL_COLOR] = document.getElementById(CHAR_CHANNEL_COLOR+"_"+c).value;
    // power
    json[CHAR_CHANNELS][c][CHAR_CHANNEL_POWER] = document.getElementById(CHAR_CHANNEL_POWER+"_"+c).value;
    // pin
    if(json[CHAR_PWM_GENERATOR] == PWM_GENERATOR_ESP8266) {
      json[CHAR_CHANNELS][c][CHAR_CHANNEL_PIN] = document.getElementById(CHAR_CHANNEL_PIN+"_"+c).value;
    }
    // max moonlight value
    if(json[CHAR_CHANNELS][c][CHAR_CHANNEL_MOONLIGHT]) {
      json[CHAR_CHANNELS][c][CHAR_CHANNEL_MAX_MOONLIGHT_VALUE] = document.getElementById(CHAR_CHANNEL_MAX_MOONLIGHT_VALUE+"_"+c).value;
    }
    // moonlight
    json[CHAR_CHANNELS][c][CHAR_CHANNEL_MOONLIGHT] = document.getElementById(CHAR_CHANNEL_MOONLIGHT+"_"+c).checked;
  }
  // channel number
  json[CHAR_NUM_OF_CHANNELS] = document.getElementById(CHAR_NUM_OF_CHANNELS).value;
  // timezone
  json[CHAR_TIMEZONE] = document.getElementById(CHAR_TIMEZONE).value;
  // pwm generator
  json[CHAR_PWM_GENERATOR] = document.getElementById(CHAR_PWM_GENERATOR).value;
  // pwm frequency
  json[CHAR_PWM_FREQUENCY] = document.getElementById(CHAR_PWM_FREQUENCY).value;
  
  displaySettings(json);
}
// saves the new settings to the server
function saveSettings() {
  json.id = ID_SAVE_SETTINGS;
  sendWebsocketMsg(JSON.stringify(json));
}


/*
 * About section
 */
function displayAbout() {
  content = "Programmable Aquarium Light Controller based on the cheap ESP8266 microcontroller.<br>";
  content += "More information can be found in the <a href=\"https://github.com/mich4el-git/ReefLight\">Github Repository</a>.";
  document.getElementById('content_div').innerHTML = content;
}


/*
 * footer
 */
content = "ReefLight Aquarium Light Controller | <a href=\"https://github.com/mich4el-git/ReefLight\">Github Repository</a> | ";
content += "Michael Dahsler 2018 | published under the <a href=\"https://opensource.org/licenses/MIT\">MIT Licence</a>";
document.getElementById('footer').innerHTML = content;
 
/*
 * opens the different sites
 */
function openContent(id) {
  console.log("open content: "+id);
  tabs = document.getElementsByClassName('tablinks');
  for(i=0;i<tabs.length;i++) {
    tabs[i].style.backgroundColor = '#E2E2E2';
  }
  document.getElementById('tab_'+id).style.backgroundColor = '#ccc';
  switch(id) {
    case 'manual':
      var tmp = {"id":ID_REQUEST_MANUAL_FROM_SERVER};
      sendWebsocketMsg(JSON.stringify(tmp));
      break;
    case 'schedule':
      var tmp = {"id":ID_REQUEST_SCHEDULE_FROM_SERVER};
      sendWebsocketMsg(JSON.stringify(tmp));
      break;
    case 'settings':
      var tmp = {"id":ID_REQUEST_SETTINGS_FROM_SERVER};
      sendWebsocketMsg(JSON.stringify(tmp));
      break;
    case 'about':
      displayAbout();
      break;
  }
}

/*
 * factory Settings
 */
function restart() {
  var tmp = {"id":ID_RESTART};
  sendWebsocketMsg(JSON.stringify(tmp));  
}
/*
 * restarts the server
 */
function factorySettings() {
  var tmp = {"id":ID_FACTORY_SETTINGS};
  sendWebsocketMsg(JSON.stringify(tmp));
  var tmp = {"id":ID_REQUEST_SETTINGS_FROM_SERVER};
  sendWebsocketMsg(JSON.stringify(tmp));
}

openContent("about");
