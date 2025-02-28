const { app, BrowserWindow } = require('electron');

let win;
app.whenReady().then(() => {
    win = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: { nodeIntegration: true }
    });
    win.loadFile('index.html')
    console.log("Started");
});