export const getStatus = async () => {
    const controller = new AbortController();
    const id = setTimeout(() => controller.abort(), 3000); // Таймаут 3 сек

    try {
        const res = await fetch('/api/status', { signal: controller.signal });
        clearTimeout(id);
        return await res.json();
    } catch (e) {
        throw new Error("Device unreachable");
    }
}

const sleep = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));

export const scanNetworks = async (): Promise<any[]> => {
    const response = await fetch('/api/scan');

    const contentType = response.headers.get("content-type");
    if (!contentType || !contentType.includes("application/json")) {
        throw new Error("Сервер вернул не JSON. Проверьте подключение к ESP32.");
    }

    if (response.status === 202) {
        await sleep(1500);
        return scanNetworks();
    }

    const data = await response.json();
    return data.networks || [];
}

export const saveWiFi = async (ssid: string, password: string) => {
    const response = await fetch('/api/save', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ ssid, password })
    });

    const data = await response.json();

    if (!response.ok) {
        throw new Error(data.error || 'Ошибка при сохранении');
    }

    return data;
}