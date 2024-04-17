import express from "express";
import { createServer } from "http";
import { Server } from "socket.io";
import { SerialPort, ReadlineParser } from 'serialport';
import cors from 'cors'
import mariadb from 'mariadb'

const app = express();
const httpServer = createServer(app)
const io = new Server(httpServer, {
    cors: {
        origin: "http://localhost:5173"
    }
})
app.use(cors({
    origin: "http://localhost:5173"
}))

const port = new SerialPort({
    path: '/dev/ttyACM0', 
    baudRate: 9600 
});
const parser = port.pipe(new ReadlineParser());

// Database config
const pool = mariadb.createPool({
    host: 'localhost',
    user: 'tomsshanley',
    database: 'gard_monitor_db'
});

// Function to send data to database
async function insertSensorData(data) {
    let conn;
    try {
        conn = await pool.getConnection();
        await conn.beginTransaction();
        await conn.query("INSERT INTO sensor_data (sensor_type, value) VALUES ('temperature', ?)", [data.temp]);
        await conn.query("INSERT INTO sensor_data (sensor_type, value) VALUES ('humidity', ?)", [data.humidity]);
        await conn.query("INSERT INTO sensor_data (sensor_type, value) VALUES ('moisture', ?)", [data.moisture]);
        await conn.commit();
        console.log('All sensor data inserted successfully');
    } catch (err) {
        console.error('Database operation failed:', err);
    } finally {
        if (conn) conn.end();
    }
}


// Websocket that Emits sensor data to frontend from serial monitor
parser.on('data', data => {
    if (data.startsWith("sensor-data ")) {
        try {
            let jsonData = JSON.parse(data.slice(12))
            // console.log('Received data: ', jsonData);
            io.emit("sensor-data", jsonData)
            insertSensorData(jsonData);
        } catch (error) {
            console.error('Failed to JSON.parse: ', error)
        }
    } else {
        console.log("Unhandled data from serial port:");
        console.log(data);
    }
    
});

// Toggle display endpoint
app.get('/toggle-display', (req, res) => {
    port.write('toggle-display\n', (err) => {
        if (err) {
            console.error('Error sending toggle-display command:', err);
            return res.status(500).send('Failed to send command');
        }
        console.log('toggle-display command sent');
        res.send('Command sent');
    });
});

// Endpoint to fetch mean values for each sensor
app.get('/sensor-averages', async (req, res) => {
    let conn;
    try {
        conn = await pool.getConnection();
        const results = await conn.query("SELECT sensor_type, AVG(value) as average FROM sensor_data GROUP BY sensor_type");
        res.json(results);
    } catch (error) {
        console.error('Failed to fetch averages:', error);
        res.status(500).send('Failed to fetch sensor averages');
    } finally {
        if (conn) conn.end();
    }
});

// Used for building
app.use(express.static('../frontend/dist'));


// Serve the application
httpServer.listen(3000, () => {
    console.log('Server listening on port 3000');
});