using System;
using System.IO.Ports;
using System.Collections.Generic;
using System.Linq;

public class SerialDataHandler
{
    private SerialPort serialPort;
    private string temperature = string.Empty;
    private string humidity = string.Empty;
    private string lightLevel = string.Empty;
    private string distance = string.Empty;
    private string motion = string.Empty;

    public SerialDataHandler(string portName, int baudRate) {
        serialPort = new SerialPort(portName, baudRate);
        serialPort.DataReceived += SerialDataReceived;
    }

    public void Start() {
        if (!serialPort.IsOpen) {
            serialPort.Open();
        }
    }

    public void Stop() {
        if (serialPort.IsOpen) {
            serialPort.Close();
        }
    }

    private async void SerialDataReceived(object sender, SerialDataReceivedEventArgs e) {
        try {
            if (!serialPort.IsOpen) {
                serialPort.Open();
            }

            string data = serialPort.ReadLine().Trim();

            // Parse the data
            var dataDict = ParseSerialData(data);

            // Store the data accordingly
            if (dataDict.ContainsKey("t")) {
                temperature = dataDict["t"];
            }
            else if (dataDict.ContainsKey("h")) {
                humidity = dataDict["h"];
            }
            else if (dataDict.ContainsKey("l")) {
                lightLevel = dataDict["l"];
            }
            else if (dataDict.ContainsKey("d")) {
                distance = dataDict["d"];
            }
            else if (dataDict.ContainsKey("m")) {
                motion = dataDict["m"];
            }

            // Send data to the WebSocket if all fields are filled
            if (temperature != string.Empty && humidity != string.Empty && lightLevel != string.Empty &&
                distance != string.Empty && motion != string.Empty) {
                var fullData = new Dictionary<string, string> {
                    { "t", temperature },
                    { "h", humidity },
                    { "l", lightLevel },
                    { "d", distance },
                    { "m", motion }
                };

                // Call WebSocketHandler to broadcast data
                await WebSocketHandler.BroadcastToWebSocketClients(fullData);

                // Reset data for next collection
                temperature = string.Empty;
                humidity = string.Empty;
                lightLevel = string.Empty;
                distance = string.Empty;
                motion = string.Empty;
            }
        }
        catch (Exception ex) {
            Console.WriteLine($"Serial Data Error: {ex.Message}");
        }
    }

    private Dictionary<string, string> ParseSerialData(string data) {
        Dictionary<string, string> dataDict = new Dictionary<string, string>();
        var parts = data.Split(new[] { " " }, StringSplitOptions.RemoveEmptyEntries);

        if (parts.Length >= 2) {
            string key = parts[0];
            string value = string.Join(" ", parts.Skip(1)); // Join all parts after the first one as the value
            dataDict[key] = value;
        }

        return dataDict;
    }
}
