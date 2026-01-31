import { useEffect, useState, type FC } from "react";
import { getStatus } from "@/api/wifi";
import { Badge } from "@/components/ui/badge";
import { Card, CardContent } from "@/components/ui/card";
import {
  Wifi,
  WifiOff,
  Globe,
  Cpu,
  Link2,
  Link2Off,
  Loader2,
} from "lucide-react";

// Обновляем интерфейс данных
interface StatusData {
  mode: string;
  savedSsid: string;
  connected: boolean;
  ip: string;
  serverFound: boolean; // Добавлено
  serverIP: string; // Добавлено
}

export const StatusCard: FC = () => {
  const [status, setStatus] = useState<StatusData | null>(null);
  const [error, setError] = useState(false);

  useEffect(() => {
    let isMounted = true;

    const pollStatus = async () => {
      try {
        const data = await getStatus();
        if (isMounted) {
          setStatus(data);
          setError(false);
        }
      } catch (err) {
        if (isMounted) {
          setError(true);
        }
      }
    };

    pollStatus();
    const interval = setInterval(pollStatus, 3000); // Опрос раз в 3 сек для отзывчивости

    return () => {
      isMounted = false;
      clearInterval(interval);
    };
  }, []);

  if (!status && !error) {
    return (
      <div className="flex items-center justify-center p-6">
        <Loader2 className="h-6 w-6 animate-spin text-muted-foreground" />
      </div>
    );
  }

  return (
    <Card className="overflow-hidden border-muted/60 shadow-md">
      <CardContent className="p-4 space-y-4">
        {/* Верхняя часть: Статус Wi-Fi */}
        <div className="flex items-center justify-between">
          <div className="flex items-center gap-2">
            <div
              className={`p-2 rounded-full ${
                status?.connected ? "bg-green-100" : "bg-yellow-100"
              }`}
            >
              {status?.connected ? (
                <Wifi className="h-4 w-4 text-green-600" />
              ) : (
                <WifiOff className="h-4 w-4 text-yellow-600" />
              )}
            </div>

            <div>
              <p className="text-xs font-medium text-muted-foreground uppercase tracking-wider leading-none mb-1">
                Система
              </p>
              <h3 className="text-sm font-bold leading-none">
                {status?.connected ? "Подключено" : "Ожидание сети"}
              </h3>
            </div>
          </div>
          <Badge
            variant={status?.connected ? "default" : "secondary"}
            className={status?.connected ? "bg-green-600" : ""}
          >
            {status?.mode || "N/A"}
          </Badge>
        </div>

        {/* Секция UDP Сервера */}
        <div
          className={`flex items-center justify-between p-2.5 rounded-lg border transition-colors ${
            status?.serverFound
              ? "bg-blue-50/50 border-blue-200"
              : "bg-muted/30 border-transparent"
          }`}
        >
          <div className="flex items-center gap-2">
            {status?.serverFound ? (
              <Link2 className="h-4 w-4 text-blue-600" />
            ) : (
              <Link2Off className="h-4 w-4 text-muted-foreground" />
            )}
            <div className="flex flex-col">
              <span className="text-[11px] font-bold uppercase text-muted-foreground leading-none mb-1">
                Главный сервер
              </span>
              <span
                className={`text-xs font-medium ${
                  status?.serverFound
                    ? "text-blue-700"
                    : "text-muted-foreground"
                }`}
              >
                {status?.serverFound ? "Связь установлена" : "Поиск в сети..."}
              </span>
            </div>
          </div>
          {status?.serverFound && (
            <Badge
              variant="outline"
              className="font-mono text-[10px] border-blue-200 text-blue-700 bg-white"
            >
              {status.serverIP}
            </Badge>
          )}
        </div>

        {/* Сетка с параметрами (IP и SSID) */}
        <div className="grid grid-cols-2 gap-4 pt-2 border-t border-muted">
          <div className="space-y-1">
            <div className="flex items-center gap-1.5 text-muted-foreground">
              <Globe className="h-3.5 w-3.5" />
              <span className="text-[11px] font-medium uppercase tracking-tight">
                IP Устройства
              </span>
            </div>
            <p className="text-sm font-mono bg-muted/50 rounded px-1.5 py-0.5 inline-block">
              {status?.ip || "0.0.0.0"}
            </p>
          </div>

          <div className="space-y-1">
            <div className="flex items-center gap-1.5 text-muted-foreground">
              <Cpu className="h-3.5 w-3.5" />
              <span className="text-[11px] font-medium uppercase tracking-tight">
                Текущая сеть
              </span>
            </div>
            <p className="text-sm font-semibold truncate">
              {status?.savedSsid || "—"}
            </p>
          </div>
        </div>

        {error && (
          <p className="text-[10px] text-destructive text-center animate-pulse font-medium">
            Ошибка связи с ESP32...
          </p>
        )}
      </CardContent>
    </Card>
  );
};
