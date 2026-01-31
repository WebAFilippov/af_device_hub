import { useState, useEffect } from "react";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { wifiSchema, type WiFiFormData } from "@/lib/schema";
import { saveWiFi, scanNetworks } from "@/api/wifi";
import { toast } from "sonner";

import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import {
  Form,
  FormControl,
  FormField,
  FormItem,
  FormLabel,
  FormMessage,
} from "@/components/ui/form";
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select";
import { Loader2, RefreshCw, Lock, LockOpen } from "lucide-react";

export function WifiSetup() {
  const [networks, setNetworks] = useState<any[]>([]);
  const [isScanning, setIsScanning] = useState(false);

  const [passwordCache, setPasswordCache] = useState<Record<string, string>>(
    {}
  );

  const form = useForm<WiFiFormData>({
    resolver: zodResolver(wifiSchema),
    defaultValues: { ssid: "", password: "" },
  });

  // Функция сканирования
  const handleScan = async () => {
    setIsScanning(true);
    try {
      const results = await scanNetworks();
      setNetworks(results);
    } catch (error) {
      toast.error("Ошибка при поиске сетей");
    } finally {
      setIsScanning(false);
    }
  };

  // Авто-сканирование при загрузке
  useEffect(() => {
    handleScan();
  }, []);

  // Следим за изменением SSID, чтобы подставлять пароль из кэша
  const currentSsid = form.watch("ssid");
  const selectedNetwork = networks.find((n) => n.ssid === currentSsid);

  const onSsidChange = (newSsid: string) => {
    // 1. Сохраняем текущий пароль для старого SSID перед переключением
    const prevSsid = form.getValues("ssid");
    const prevPass = form.getValues("password");
    if (prevSsid) {
      setPasswordCache((prev) => ({ ...prev, [prevSsid]: prevPass }));
    }

    // 2. Устанавливаем новый SSID
    form.setValue("ssid", newSsid);

    // 3. Достаем пароль из кэша для новой сети, если он там есть
    form.setValue("password", passwordCache[newSsid] || "");
  };

  const onSubmit = async (values: WiFiFormData) => {
    try {
      await saveWiFi(values.ssid, values.password);
      toast.success("Настройки сохранены! ESP32 перезагружается...");
    } catch (error: any) {
      const errorKey = error.message;
      if (errorKey === "empty_ssid") {
        form.setError("ssid", { message: "Выберите сеть" });
      } else {
        toast.error(`Ошибка сервера: ${errorKey}`);
      }
    }
  };

  return (
    <div className="max-w-md mx-auto p-6 bg-card rounded-xl border shadow-sm space-y-6">
      <div className="flex items-center justify-between">
        <h2 className="text-xl font-semibold">Настройка Wi-Fi</h2>
        <Button
          variant="ghost"
          size="icon"
          onClick={handleScan}
          disabled={isScanning}
        >
          <RefreshCw
            className={`h-4 w-4 ${isScanning ? "animate-spin" : ""}`}
          />
        </Button>
      </div>

      <Form {...form}>
        <form onSubmit={form.handleSubmit(onSubmit)} className="space-y-4">
          {/* Выбор сети через Select */}
          <FormField
            control={form.control}
            name="ssid"
            render={({ field }) => (
              <FormItem>
                <FormLabel>Доступные сети</FormLabel>
                <Select
                  onValueChange={onSsidChange}
                  defaultValue={field.value}
                  value={field.value}
                >
                  <FormControl>
                    <SelectTrigger>
                      <SelectValue
                        placeholder={isScanning ? "Поиск..." : "Выберите Wi-Fi"}
                      />
                    </SelectTrigger>
                  </FormControl>
                  <SelectContent>
                    {networks.map((net) => (
                      <SelectItem key={net.ssid} value={net.ssid}>
                        <div className="flex items-center justify-between w-full gap-2">
                          <span className="flex items-center gap-2">
                            {net.secure ? (
                              <Lock className="h-3 w-3" />
                            ) : (
                              <LockOpen className="h-3 w-3 text-muted-foreground" />
                            )}
                            {net.ssid}
                          </span>
                          <span className="text-[10px] opacity-50">
                            {net.rssi} dBm
                          </span>
                        </div>
                      </SelectItem>
                    ))}
                  </SelectContent>
                </Select>
                <FormMessage />
              </FormItem>
            )}
          />

          {/* Поле пароля (скрываем или дизейблим, если сеть открытая) */}
          <FormField
            control={form.control}
            name="password"
            render={({ field }) => (
              <FormItem>
                <FormLabel>Пароль</FormLabel>
                <FormControl>
                  <Input
                    {...field}
                    type="password"
                    placeholder={
                      selectedNetwork && !selectedNetwork.secure
                        ? "Сеть без пароля"
                        : "Введите пароль"
                    }
                    disabled={selectedNetwork && !selectedNetwork.secure}
                  />
                </FormControl>
                <FormMessage />
              </FormItem>
            )}
          />

          <Button
            type="submit"
            className="w-full"
            disabled={form.formState.isSubmitting}
          >
            {form.formState.isSubmitting && (
              <Loader2 className="mr-2 h-4 w-4 animate-spin" />
            )}
            Сохранить настройки
          </Button>
        </form>
      </Form>
    </div>
  );
}
