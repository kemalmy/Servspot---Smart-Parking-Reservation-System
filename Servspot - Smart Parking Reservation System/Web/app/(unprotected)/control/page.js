"use client";

import { useState, useEffect } from "react";

const WEBSOCKET_URL = "ws://192.168.1.37:81";

export default function SignInPage() {
  const [websocket, setWebsocket] = useState(null);
  const [sensorReadings, setSensorReadings] = useState([]);

  useEffect(() => {
    const ws = new WebSocket(WEBSOCKET_URL);

    const handleOpen = () => {
      console.log("WebSocket connection opened");
    };

    const handleMessage = (event) => {
      try {
        const jsonData = JSON.parse(event.data);
        setSensorReadings(jsonData.sensorReadings);
      } catch (error) {
        console.error("Error parsing JSON:", error);
      }
    };

    const handleClose = () => {
      console.log("WebSocket connection closed");
    };

    ws.addEventListener("open", handleOpen);
    ws.addEventListener("message", handleMessage);
    ws.addEventListener("close", handleClose);

    setWebsocket(ws);

    return () => {
      ws.removeEventListener("open", handleOpen);
      ws.removeEventListener("message", handleMessage);
      ws.removeEventListener("close", handleClose);
      ws.close();
    };
  }, []);

  const getColorForDistance = (distance) => {
    if (distance < 200) {
      return "green";
    } else if (distance < 500) {
      return "yellow";
    } else if (distance < 1000) {
      return "orange";
    } else {
      return "red";
    }
  };

  return (
    <div>
      <h1>Sensor Readings</h1>
      <ul>
        {sensorReadings.map((reading, index) => (
          <li key={index}>
            Sensor {reading.sensorNumber}: {reading.distance} units. Color{" "}
            {getColorForDistance(reading.distance)}
          </li>
        ))}
      </ul>
    </div>
  );
}
