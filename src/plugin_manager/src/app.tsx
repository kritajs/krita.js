import * as React from "react";
import { createRoot } from "react-dom/client";

function App() {
  return (
    <button onClick={() => console.log("BUTTON CLICKED")}>Click me!</button>
  );
}

const root = createRoot(document.getElementById("root")!);
root.render(<App />);
