using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using InTheHand.Net.Sockets;
using InTheHand.Net.Bluetooth;
using System.Text;
using System.Runtime.CompilerServices;

public class BlueToothDataHandler
{
    private string temperature = string.Empty;
    private string humidity = string.Empty;
    private string lightLevel = string.Empty;
    private string distance = string.Empty;
    private string motion = string.Empty;

    private BluetoothClient bluetoothClient;
    private BluetoothDeviceInfo bluetoothDeviceInfo;

    public BlueToothDataHandler() {
        bluetoothClient = new BluetoothClient();
    }

    public async Task StartAsync() {
        Console.WriteLine(bluetoothClient.RemoteMachineName);
        try {
            if (bluetoothClient.Connected) {
                // Console.WriteLine("BlueTooth Device is connected");
                await BlueToothDataReceived();
            } else {
                // Wait until Bluetooth device is found
                await WaitForBluetoothDeviceAsync();
                await BlueToothDataReceived();
            }
        } catch (Exception ex) {
            // Console.WriteLine($"Error with Bluetooth: {ex.Message});
        }
    }

    public void Stop() {
        if (bluetoothClient.Connected) {
            bluetoothClient.Close();
        }
    }

    private async Task WaitForBluetoothDeviceAsync() {
        Console.WriteLine("Scanning for Bluetooth devices...");

        // Continue scanning until device is found
        while (true) {
            var devices = bluetoothClient.DiscoverDevices();

            foreach (var device in devices) {
                Console.WriteLine($"Found device: {device.DeviceName} - {device.DeviceAddress}");

                // Replace this with your Arduino Nano's Bluetooth address or a matching condition
                if (device.DeviceName.Contains("RugerProbe")) { //|| device.DeviceAddress.ToString().Contains("B0:B2:1C:56:7F:D6")) {
                    bluetoothDeviceInfo = device;
                    bluetoothClient.Connect(bluetoothDeviceInfo.DeviceAddress, BluetoothService.SerialPort);
                    Console.WriteLine($"Connected to Bluetooth device: {device.DeviceName} ({device.DeviceAddress})");
                    return; // Stop scanning once the device is found and connected
                }
            }

            // Wait before scanning again
            await Task.Delay(1000); // 1-second delay between scans
        }
    }

    private async Task BlueToothDataReceived() {
        if (bluetoothClient.Connected) {

            var netStream = bluetoothClient.GetStream();
            byte[] buff = new byte[1024];
            
            while (bluetoothClient.Connected) {
                try {

                    int bytesRead = await netStream.ReadAsync(buff, 0, buff.Length);

                    if (bytesRead > 0) {
                        // Parse the data
                        string data = Encoding.ASCII.GetString(buff, 0, bytesRead).Trim();
                        var dataDict = ParseBlueToothData(data);

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
            
                }
                catch (Exception ex) {
                    Console.WriteLine($"Bluetooth Data Error: {ex.Message}");
                }
            }
        }
    }

    private Dictionary<string, string> ParseBlueToothData(string data) {
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
