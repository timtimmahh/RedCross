/**
 * Complete the HTML card tag. Called when
 * makeCards(Sensor& sensor, Sensors&... sensors) has no more sensors to set up.
 *
 * @return a new line following the cards
 */
String makeCards() {
  return "\n";
}


/**
 * Creates the card items for each sensor to display on the web page.
 *
 * @tparam Sensor at least one sensor required
 * @tparam Sensors any additional sensors
 * @param sensor a required sensor name
 * @param sensors additional sensor names
 * @return HTML div.card tags for each sensor
 */
template <typename Sensor, typename... Sensors>
String makeCards(Sensor& sensor, Sensors&... sensors) {
  return (String(R"=====(
<div class="card">
<h4>The ESP32 )=====") + sensor + R"=====( Card</h4><br>
<h1>)=====" + sensor
                      + R"=====( Data: <span id=")=====" + sensor + R"=====(Value">0</span></h1><br>
</div>
)=====" + makeCards(sensors...));
}

/**
 * Complete the timer interval function. Called when
 * popIntervalFunc(Sensor& sensor, Sensors&... sensors) has no more sensors to
 * set up.
 *
 * @return the end of the interval function.
 */
String popIntervalFunc() {
  return "}, 2000);";
}

/**
 * Creates a timer interval that attempts to obtain sensor data for all
 * configured sensors.
 *
 * @tparam Sensor the sensors to set up
 * @tparam Sensors the names of the sensors
 * @param sensor require at least one sensor
 * @param sensors any additional sensors
 * @return a JavaScript function for obtaining sensor data
 */
template <typename Sensor, typename...Sensors>
String popIntervalFunc(Sensor& sensor, Sensors&... sensors) {
  return (String("  getData(\"") + sensor + "\");\n" + popIntervalFunc(sensors...));
}

/**
 * Builds the HTML page with sections for each configured sensor instance.
 *
 * @tparam Sensors the sensors to display on the page
 * @param sensors the sensor names
 * @return the HTML page
 */
template <typename... Sensors>
String makeHTMLPage(Sensors&... sensors) {
  return (String(R"=====(
<!DOCTYPE html>
<html>
<style>
.card{
    max-width: 400px;
     min-height: 250px;
     background: #02b875;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}
</style>
<body>
)=====") + makeCards(sensors...)
    + "<script>\nsetInterval(function() {\n" + popIntervalFunc(sensors...)
    + R"=====(
function getData(name) {
  var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById(name + "Value").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "read" + name, true);
    xhttp.send();
};
</script>
</body>
</html>
)=====");
}

#define MAKE_HTML(sensors...) makeDataScript(sensors)

/**
 * The HTML and JavaScript for displaying sensor data on ESP32. Marked with
 * PROGMEM specifier so the data is compiled into ESP32 flash memory, otherwise
 * the whole system would likely crash.
 */
const String MAIN_page[] PROGMEM = {MAKE_HTML("Lidar", "AccelGyro")};