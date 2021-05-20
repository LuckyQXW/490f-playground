(function() {
    "use strict";
    window.addEventListener("load", init);
    const serial = new Serial();

    function init() {
        // Setup Web Serial using serial.js
        serial.on(SerialEvents.CONNECTION_OPENED, onSerialConnectionOpened);
        serial.on(SerialEvents.CONNECTION_CLOSED, onSerialConnectionClosed);
        serial.on(SerialEvents.DATA_RECEIVED, onSerialDataReceived);
        serial.on(SerialEvents.ERROR_OCCURRED, onSerialErrorOccurred);

        function onSerialErrorOccurred(eventSender, error) {
            console.log("onSerialErrorOccurred", error);
        }

        function onSerialConnectionOpened(eventSender) {
            console.log("onSerialConnectionOpened", eventSender);
            id("connect-button").classList.add("hidden");
            id("slider-container").classList.remove("hidden");
        }

        function onSerialConnectionClosed(eventSender) {
            console.log("onSerialConnectionClosed", eventSender);
            id("connect-button").classList.remove("hidden");
            id("slider-container").classList.add("hidden");
        }

        function onSerialDataReceived(eventSender, newData) {
            id('slider').disabled = false;
            console.log("onSerialDataReceived", newData);
            id("move-indicator").classList.add("hidden");
        }

        id("slider").addEventListener("change", onSliderValueChanged);
        id("connect-button").addEventListener("click", onConnectButtonClick)
    }

    async function onConnectButtonClick() {
        console.log("Connect button clicked!");
        if (navigator.serial) {
            if (!serial.isOpen()) {
                await serial.connectAndOpen();
            } else {
                console.log('Serial connection already opened');
            }
        } else {
            alert('web serial api unavailable');
        }
    }

    function onSliderValueChanged() {
        var slider = id("slider");
        console.log("Writing to serial: ", slider.value.toString());
        serial.writeLine(slider.value);
        id("slider-value").textContent = slider.value;
        slider.disabled = true;
        id("move-indicator").classList.remove("hidden");
    }

    /**
     * Helper method for getting element by id
     * @param {String} elementID - the id with which the target objects are attached to
     * @return {Object} the DOM element object with the specified id
     */
    function id(elementID) {
        return document.getElementById(elementID);
    }

})();