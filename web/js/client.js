document.onreadystatechange = function () {
  console.log(document.readyState);
  if (document.readyState === "complete") {
    const PIN = document.getElementById("pin");
    const SIGNAL = document.getElementById("signal");
    const MODE = document.getElementById("mode");
    const DIGITAL = document.getElementById("digital");
    const ANALOG = document.getElementById("analog");
    const BTN = document.getElementById("button");
    const STATUS = document.getElementById("status");
    const RPIN = document.getElementById("rpin");
    const RSIGNAL = document.getElementById("rsignal");
    const RMODE = document.getElementById("rmode");
    const RDIGITAL = document.getElementById("rdigital");
    const RANALOG = document.getElementById("ranalog");
    const API = "http://192.168.1.228:5555/api";

    /**
     * postRequest
     * @param {*} parseOut - function to prepare request
     * @param {*} parseIn - function to format response
     */
    const sendRequest = function (parseOut, parseIn) {
      var xhr = new XMLHttpRequest();
      xhr.open("POST", API, true);
      xhr.onload = function () {
        if (this.status == 200) {
          parseIn(this.responseText);
          STATUS.innerHTML = this.status + " - OK";
        } else if (this.status == 404) {
          STATUS.innerHTML = this.status + "- Not Found";
        }
      };
      xhr.onerror = function () {
        console.log("ERR:READYSTATE: ", this.readyState, this.status);
      };
      // Send request
      xhr.send(parseOut());
    };

    /**
     * pinOut - prepares a pin request
     * @returns JSON string
     */
    const pinOut = function () {
      return JSON.stringify({
        pin: parseInt(PIN.value),
        mode: MODE.value,
        signal: SIGNAL.value,
        digital: parseInt(DIGITAL.value),
        analog: parseInt(ANALOG.value),
      });
    };

    /**
     * pinIn parses and formats incoming JSON text
     * @param {*} responseText - text to be formatted
     */
    const pinIn = function (responseText) {
      var resp = JSON.parse(responseText);
      console.log(resp);
      RPIN.innerHTML = resp.pin;
      RSIGNAL.innerHTML = resp.signal;
      RMODE.innerHTML = resp.mode;
      RDIGITAL.innerHTML = resp.digital;
      RANALOG.innerHTML = resp.analog;
    };

    const sendPinRequest = function () {
      sendRequest(pinOut, pinIn);
    };
    /* 
    Create event listeners after the functions
    they refer to are defined
    */
    BTN.addEventListener("click", sendPinRequest);
  }
};
