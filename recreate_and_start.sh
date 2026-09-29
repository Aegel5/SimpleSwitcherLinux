#!/usr/bin/env bash

# Включаем строгий режим: скрипт упадет при любой ошибке
set -e

# ==================== КОНФИГУРАЦИЯ ====================
# Укажите точный путь к вашему портативному приложению
APP_PATH="./release/SimpleSwitcher"
SERVICE_NAME="SimpleSwitcher.service"
# ======================================================

# 1. Защита: проверяем, существует ли вообще бинарник
if [ ! -f "$APP_PATH" ]; then
    echo "Ошибка: Исполняемый файл не найден по пути: $APP_PATH" >&2
    exit 1
fi

# Превращаем путь в абсолютный, чтобы у systemd не было проблем с запуском
APP_PATH=$(realpath "$APP_PATH")

# 2. Вычисляем текущий SHA-256 файла (вырезаем только саму строку хэша)
CURRENT_SHA256=$(sha256sum "$APP_PATH" | awk '{print $1}')
echo "Успешно посчитан SHA-256: $CURRENT_SHA256"

# 3. Путь к системному юниту
TARGET_PATH="/etc/systemd/system/$SERVICE_NAME"

# Генерируем тело сервиса с правильными зависимостями для устройств ввода
sudo tee "$TARGET_PATH" > /dev/null << EOF
[Unit]
Description=My Automated Portable Input Service
# Гарантируем, что клавиатуры и uinput уже инициализированы ядром перед стартом
Wants=systemd-udev-settle.service
After=systemd-udev-settle.service

[Service]
Type=simple

# Запрещаем сервису изменять файлы в /usr, /boot, /etc
ProtectSystem=strict

# Умная проверка хэша перед стартом
ExecStartPre=/bin/bash -c 'echo "$CURRENT_SHA256  $APP_PATH" | sha256sum --check'

# Запуск от root с доступом к созданию, чтению и захвату устройств ввода
ExecStart=$APP_PATH

[Install]
WantedBy=multi-user.target
EOF

# Выставляем права: только root может изменять этот файл (защита от вирусов без root-прав)
sudo chown root:root "$TARGET_PATH"
sudo chmod 644 "$TARGET_PATH"

# 5. Применяем изменения, добавляем в автозагрузку и запускаем сервис
echo "Перезагружаем демона systemd..."
sudo systemctl daemon-reload

echo "Добавляем сервис $SERVICE_NAME в автозагрузку системы..."
sudo systemctl enable "$SERVICE_NAME"

echo "Запускаем/перезапускаем сервис $SERVICE_NAME..."
sudo systemctl restart "$SERVICE_NAME"

echo "Все готово! Сервис успешно создан, добавлен в автозагрузку, запущен от root и защищен от подмены файлы."
