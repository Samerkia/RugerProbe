const ws = new WebSocket("ws://localhost:8080");

// Event handler when the WebSocket connection is established
ws.onopen = () => {
    console.log("WebSocket connection established!");
};

ws.onmessage = (event) => {
    console.log("Received data: " + event.data);

    // Parse the incoming JSON data
    const data = JSON.parse(event.data);

    // Update each value (temperature, humidity, light, distance, motion) based on the data received
    updateTemperature(data["t"], data["h"]);
    updateLightLevels(data["l"]);
    document.getElementById("distance").innerText = data["d"] || "N/A";
    document.getElementById("motion").innerText = data["m"] || "N/A";
};

// Update the tempeture and humidity as well as the display circle
function updateTemperature(temp, hum) {
    let maxTemp = 212; // Max Temperature in fahrenheit to get percent 
    let tempPercent = Math.min(Math.max(parseFloat(temp), 0), maxTemp) / maxTemp; // get the percent 
    let circleLength = 251.2; // Circle circumfrence
    let dashOffset = circleLength * (1 - tempPercent); 

    document.getElementById("tempCircle").style.strokeDashoffset = dashOffset;
    document.getElementById("temperature").textContent = temp || "N/A";
    document.getElementById("humidity").textContent = hum || "N/A";
}

// update the light level as well as the display circle
function updateLightLevels(light) {
    let maxLight = 512; // Max light level to get percent 
    let lightPercent = Math.min(Math.max(parseInt(light), 0), maxLight) / maxLight; // get the percent 
    let circleLength = 251.2; // Circle circumfrence
    let dashOffset = circleLength * (1 - lightPercent); 

    document.getElementById("lightCircle").style.strokeDasharray = dashOffset;
    document.getElementById("lightLevel").textContent = light || "N/A";
}

// Event handler for WebSocket errors
ws.onerror = (error) => {
    console.log("WebSocket error: " + error);
};

// Event handler for when the WebSocket connection is closed
ws.onclose = () => {
    console.log("WebSocket connection closed.");
};