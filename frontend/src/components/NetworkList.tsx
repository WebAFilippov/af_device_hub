import type { INetworks } from "@/app/App";
import { Button } from "@/components/ui/button";

export const NetworkList = ({
  networks,
  onSelect,
}: {
  networks: INetworks[];
  onSelect: (ssid: string) => void;
}) => {
  return (
    <div className="space-y-2">
      {networks.map((n) => (
        <Button
          key={n.ssid}
          variant="outline"
          className="w-full justify-between"
          onClick={() => onSelect(n.ssid)}
        >
          <span>{n.ssid}</span>
          <span>
            {n.secure ? "🔒" : "🔓"} {n.rssi}
          </span>
        </Button>
      ))}
    </div>
  );
};
