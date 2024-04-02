# Используйте официальный образ Windows Server Core
FROM mcr.microsoft.com/windows/servercore:ltsc2019

# Копируйте исходный код в контейнер
WORKDIR /app
COPY client.cpp .

# Установите компилятор и библиотеку Winsock
RUN curl -o VC_redist.x64.exe https://aka.ms/vs/16/release/VC_redist.x64.exe && \
    VC_redist.x64.exe /quiet /install && \
    del VC_redist.x64.exe

# Компиляция исходного кода
RUN cl /EHsc client.cpp /link /out:client.exe

# Запуск приложения при запуске контейнера
CMD ["client.exe"]
