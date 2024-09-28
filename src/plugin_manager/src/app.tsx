import * as React from "react";
import { createRoot } from "react-dom/client";

function onClick() {
  console.log("JS BUTTON CLICKED");
  console.log(new Q.Object());
  console.log(new Q.PushButton());
  console.log(new K.Krita());
}

function App() {
  return <button onClick={onClick}>Click me!</button>;
}

const root = createRoot(document.getElementById("root")!);
root.render(<App />);
