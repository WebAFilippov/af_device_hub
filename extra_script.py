
Import("env")
import shutil
import os

def before_build_littlefs(source, target, env):
    print("Building React App...")
    env.Execute("cd frontend && npm run build")
    print("React App built!")

    # Папка для данных LittleFS
    data_dir = "data"

    # Удаляем старую папку data
    print("Removing old LittleFS image...")
    if os.path.exists(data_dir):
        shutil.rmtree(data_dir)

    # Создаём папку data
    print("Creating new data directory...")
    os.makedirs(data_dir, exist_ok=True)

    # Копируем .gz файлы и main.ico из front/dist
    print("Copying files to LittleFS...")
    dist_dir = "frontend/dist"
    files_to_copy = ["index.html.gz",
                     "main.css.gz", "main.js.gz", "favicon.ico.gz"]
    for file in files_to_copy:
        src_path = os.path.join(dist_dir, file)
        dst_path = os.path.join(data_dir, file)
        if os.path.exists(src_path):
            shutil.copy(src_path, dst_path)
            print(f"Copied {file} to {data_dir}")
        else:
            print(f"Warning: {file} not found in {dist_dir}")

    # Проверка, что все файлы скопированы
    missing_files = [f for f in files_to_copy if not os.path.exists(
        os.path.join(data_dir, f))]
    if missing_files:
        print(
            f"Error: Missing files in {data_dir}: {', '.join(missing_files)}")
        raise Exception("Build failed due to missing files")


env.AddPreAction("$BUILD_DIR/littlefs.bin", before_build_littlefs)
