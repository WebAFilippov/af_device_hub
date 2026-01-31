import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { wifiSchema, type WiFiFormData } from "@/lib/schema";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import {
  Form,
  FormControl,
  FormField,
  FormItem,
  FormLabel,
  FormMessage,
} from "./ui/form";
import { useState } from "react";
import { saveWiFi } from "@/api/wifi";
import { toast } from "sonner";

export function WifiForm() {
  const form = useForm<WiFiFormData>({
    resolver: zodResolver(wifiSchema),
    defaultValues: { ssid: "", password: "" },
  });
  const [isLoading, setIsLoading] = useState(false);

  const onSumbit = async (values: WiFiFormData) => {
    setIsLoading(true);
    try {
      await saveWiFi(values.ssid, values.password);
      toast.success("Настройки сохранены! ESP32 перезагружается...");
    } catch (error: any) {
      const errorKey = error.message;
      if (errorKey === "empty_ssid") {
        form.setError("ssid", {
          type: "server",
          message: "Бэкенд сообщает, что SSID пуст",
        });
      } else if (errorKey === "invalid_json") {
        alert("Ошибка протокола передачи данных");
      } else {
        form.setError("root", { message: `Ошибка сервера: ${errorKey}` });
      }
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <Form {...form}>
      <form onSubmit={form.handleSubmit(onSumbit)} className="space-y-4">
        <FormField
          control={form.control}
          name="ssid"
          render={({ field }) => (
            <FormItem>
              <FormLabel>SSID</FormLabel>
              <FormControl>
                <Input {...field} />
              </FormControl>
              <FormMessage />
            </FormItem>
          )}
        />
        <FormField
          control={form.control}
          name="password"
          render={({ field }) => (
            <FormItem>
              <FormLabel>Пароль</FormLabel>
              <FormControl>
                <Input {...field} />
              </FormControl>
              <FormMessage />
            </FormItem>
          )}
        />

        {form.formState.errors.root && (
          <p className="text-sm font-medium text-destructive">
            {form.formState.errors.root.message}
          </p>
        )}

        <Button type="submit" disabled={isLoading} className="w-full">
          {isLoading ? "Сохранение..." : "Сохранить и перезагрузить"}
        </Button>
      </form>
    </Form>
  );
}
