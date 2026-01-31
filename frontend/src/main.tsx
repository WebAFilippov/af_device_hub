import { createRoot } from "react-dom/client";
import "./index.css";
import { ThemeProvider } from "./components/ThemeProvider";
import App from "./app/App";
import { Toaster } from "./components/ui/sonner";

createRoot(document.getElementById("root")!).render(
  <ThemeProvider defaultTheme="system" storageKey="vite-ui-theme">
    <Toaster />
    <App />
  </ThemeProvider>
);
