import { scanNetworks } from "@/api/wifi";
import { useState } from "react";
import { Button } from "./ui/button";

export function WifiScanner() {
  const [networks, setNetworks] = useState<any[]>([]);
  const [isScanning, setIsScanning] = useState(false);

  const handleScan = async () => {
    setIsScanning(true);
    try {
      const results = await scanNetworks();
      setNetworks(results);
    } catch (error) {
      console.error("Ошибка сканирования", error);
    } finally {
      setIsScanning(false);
    }
  };

  return (
    <div className="space-y-4">
      <Button onClick={handleScan} disabled={isScanning}>
        {isScanning ? "Поиск сетей..." : "Обновить список сетей"}
      </Button>

      <div className="grid gap-2">
        {networks.map((net) => (
          <div
            key={net.ssid}
            className="p-2 border rounded flex justify-between"
          >
            <span>{net.ssid}</span>
            <span className="text-gray-400">{net.rssi} dBm</span>
          </div>
        ))}
      </div>
    </div>
  );
}
