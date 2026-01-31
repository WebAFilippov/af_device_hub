import { StatusCard } from "@/components/StatusCard";
import { ModeToggle } from "@/components/ModeToggle";

import { WifiSetup } from "@/components/WiFiSettings";

export type IStatus = {
  mode: string;
  savedSsid: string;
  connected: boolean;
  ip: string;
};

export type INetworks = {
  ssid: string;
  rssi: string;
  secure: boolean;
};

export default function App() {
  return (
    <div className="min-h-screen p-4 bg-background text-foreground space-y-4">
      <div className="flex justify-end">
        <ModeToggle />
      </div>

      <main className="container max-w-lg mx-auto py-10 px-4 space-y-6">
        <StatusCard />
        <WifiSetup />
      </main>
      {/* <WifiScanner />

      <WifiForm  /> */}
    </div>
  );
}
