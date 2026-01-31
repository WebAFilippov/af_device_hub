import z from "zod";

export const wifiSchema = z.object({
    ssid: z.string().min(1, 'Выберите сеть'),
    password: z.string().transform(v => v === "" ? "" : v).refine(v => v === "" || v.length >= 8, {
        message: 'Пароль должен быть не менее 8 символов'
    })
});

export type WiFiFormData = z.infer<typeof wifiSchema>;