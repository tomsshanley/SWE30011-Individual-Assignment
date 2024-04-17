<script setup>
import { ref } from 'vue'
import {socket} from '@/socket.js'
console.log("Hello");
const temp = ref(0)
const moisture = ref(0)
const humidity = ref(0)
const tempAvg = ref(0)
const moistureAvg= ref(0)
const humidityAvg = ref(0)
socket.off()

function connect(){
  socket.connect();
}
function disconnet() {
  socket.disconnet();
}

socket.on('sensor-data', (data) => {
  if (data) {
    temp.value = data.temp
    moisture.value = data.moisture
    humidity.value = data.humidity
  }
})

async function toggleDisplay() {
  try {
    const response = await fetch('http://localhost:3000/toggle-display')
    console.log(response);
  } catch (error) {
    console.log(error);
    
  }
}
async function calcalateAverages() {
  try {
    const response = await (await fetch('http://localhost:3000/sensor-averages')).json()
    console.log(response);
    tempAvg.value = response[2].average
    humidityAvg.value = response[0].average
    moistureAvg.value = response[1].average
  } catch (error) {
    console.log(error);
    
  }
}
</script>

<template>
  <div class="container">
    <div class="temp-card">
      <h1>Temperature</h1>
      <h2>{{ temp }} C</h2>
      <h3>Average: {{ tempAvg }}</h3>
    </div>
    <div class="humidity-card">
      <h1>Humidity</h1>
      <h2>{{ humidity }} C</h2>
      <h3>Average: {{ humidityAvg }}</h3>
    </div>
    <div class="moisture-card">
      <h1>Moisture</h1>
      <h2>{{ moisture }} C</h2>
      <h3>Average: {{ moistureAvg }}</h3>
    </div>
    <!-- <button @click="connect()">Connect</button> -->
  <!-- <button @click="disconnect()">Disconnect</button> -->
  <button class="toggleDisplayButton" @click="toggleDisplay()">Toggle 7 segment display</button>
  <button class="calcalateAveragesButton" @click="calcalateAverages()">Calculate Averages</button>
  </div>

  
</template>

<style scoped>
.container {
  display: flex;
  justify-content: space-around; /* This will distribute space around the cards equally */
  align-items: center; /* This will vertically align the cards in the center */
  height: 100vh; /* Sets the container height to the full viewport height */
  margin: 0 auto; /* Centers the container horizontally */
  width: 100%
}

.temp-card, .humidity-card, .moisture-card {
  flex: 1;  /* This gives each card equal width */
  margin: 10px; /* Optional: Adds some space between the cards */
  text-align: center; /* Centers the text inside each card */
}

/* Optional: Adds some basic styling to the cards */
.temp-card, .humidity-card, .moisture-card {
  border: 1px solid #ccc;
  padding: 20px;
  box-shadow: 0px 0px 10px #ccc;
}
.toggleDisplayButton {
  position: fixed; /* Keeps the button fixed during scrolling */
  top: 10px;       /* Distance from the top of the viewport */
  right: 10px;     /* Distance from the right of the viewport */
  padding: 10px 20px;
  background-color: #007BFF;
  color: white;
  border: none;
  border-radius: 5px;
  cursor: pointer;
  outline: none;
}
.calcalateAveragesButton {
  position: fixed; /* Keeps the button fixed during scrolling */
  top: 10px;       /* Distance from the top of the viewport */
  left: 10px;     /* Distance from the right of the viewport */
  padding: 10px 20px;
  background-color: #007BFF;
  color: white;
  border: none;
  border-radius: 5px;
  cursor: pointer;
  outline: none;
}
button:hover {
  background-color: #0056b3;
}
</style>
