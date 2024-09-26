import * as React from "react";
import { createRoot } from "react-dom/client";

function onClick() {
  console.log("JS BUTTON CLICKED");
  // const core = new Qt5Core();
  // const qobj = new core.QObject();
}

function App() {
  return <button onClick={onClick}>Click me!</button>;
}

const root = createRoot(document.getElementById("root")!);
root.render(<App />);
