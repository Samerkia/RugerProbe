const ws = new WebSocket("ws://localhost:8080");

// Event handler when the WebSocket connection is established
ws.onopen = () => {
    console.log("WebSocket connection established!");
};

ws.onmessage = (event) => {
    console.log("Received data: " + event.data);

    // Parse the incoming JSON data
    const data = JSON.parse(event.data);

    // Update each value (temperature, humidity, light) based on the data received
    // If the value is not available, fallback to "N/A"
    document.getElementById("temperature").innerText = data["t"] || "N/A";
    document.getElementById("humidity").innerText = data["h"] || "N/A";
    document.getElementById("lightLevel").innerText = data["l"] || "N/A";
    document.getElementById("distance").innerText = data["d"] || "N/A";
};



// Event handler for WebSocket errors
ws.onerror = (error) => {
    console.log("WebSocket error: " + error);
};

// Event handler for when the WebSocket connection is closed
ws.onclose = () => {
    console.log("WebSocket connection closed.");
};