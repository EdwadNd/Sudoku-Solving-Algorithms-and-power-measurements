const axios = require('axios');
const fs = require('fs');
const { parse } = require('json2csv');

const API_URL = 'http://192.168.123.123/api/dev'; // Replace with your actual API endpoint
const CSV_FILE_PATH = 'Nothingisrunn.csv';
const INTERVAL = 1000; // 1 seconds

// Function to extract all metrics from a measurement object
function extractMeasurements(measurements) {
    const result = {};
    for (const [key, measurement] of Object.entries(measurements)) {
        result[`${measurement.type}_${key}`] = measurement.value;
    }
    return result;
}

// Function to extract data from the API response
function extractData(jsonData) {
    if (jsonData.retCode === 0 && jsonData.data) {
        let deviceId = Object.keys(jsonData.data)[0];
        let device = jsonData.data[deviceId];
        
        let extractedData = {
            timestamp: new Date().toISOString(),
            deviceId: deviceId,
            deviceName: device.name,
            deviceLabel: device.label,
            deviceState: device.state,
            deviceType: device.type,
        };

        // Extract Circuit 1 and Circuit 2 currents
        extractedData['circuit1_current'] = device.entity.breaker0.measurement[4].value;
        extractedData['circuit2_current'] = device.entity.breaker1.measurement[4].value;

        // Extract total measurements
        extractedData['total_realPower'] = device.entity.total0.measurement[0].value;
        extractedData['total_powerFactor'] = device.entity.total0.measurement[2].value;
        extractedData['total_energy'] = device.entity.total0.measurement[3].value;

        // Extract Phase A measurements
        extractedData['phaseA_voltage'] = device.entity.phase0.measurement[0].value;
        extractedData['phaseA_current'] = device.entity.phase0.measurement[4].value;
        extractedData['phaseA_apparentPower'] = device.entity.phase0.measurement[9].value;
        extractedData['phaseA_currentCrestFactor'] = device.entity.phase0.measurement[14].value;

        return extractedData;
    }
    throw new Error("Failed to extract data. Response code: " + jsonData.retCode);
}

// Function to append data to CSV file
function appendToCSV(data) {
    const csv = parse(data, { header: !fs.existsSync(CSV_FILE_PATH) });
    fs.appendFileSync(CSV_FILE_PATH, csv + '\n');
    console.log('Data appended to CSV ');
}

// Main function to fetch data and log it
async function fetchAndLogData() {
    try {
        const response = await axios.get(API_URL);
        const extractedData = extractData(response.data);
        appendToCSV(extractedData);
    } catch (error) {
        console.error('Error:', error.message);
    }
}

// Start the data extraction process
console.log('Starting data extraction...');
setInterval(fetchAndLogData, INTERVAL);