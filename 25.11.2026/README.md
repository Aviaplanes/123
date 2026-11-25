# Найди пару — ПК, 25.11.2026

Две реализации одной небольшой игры для **Windows 10/11 x64**:

- **Готовый [`MemoryGame.exe`](MemoryGame.exe)** — нативная версия на C / WinAPI,
  собранная прямо в Linux-песочнице кросс-компилятором Zig 0.14.1.
  Исходник: [`native/MemoryGame.c`](native/MemoryGame.c).
- **Исходная C# / Windows Forms версия** — `Program.cs` и `MemoryGame.csproj`.
  Она сохранена без изменений и собирается существующим GitHub Actions workflow.
  EXE в этой папке — именно нативная версия, не артефакт Actions.

## Скачать и играть

Открой [`MemoryGame.exe`](MemoryGame.exe) на GitHub и нажми **Download raw file**.
Запусти скачанный EXE на Windows. Не нужны .NET, Python, Zig или дополнительные DLL:
игра использует только системные библиотеки Windows. Файл не подписан сертификатом,
поэтому SmartScreen может показать предупреждение.

- 16 карточек, 8 пар эмодзи; новая случайная раскладка при каждом старте.
- Открой две карточки: одинаковые остаются открытыми, разные закрываются через 750 мс.
- Пока показывается несовпавшая пара, другие карточки не открываются.
- Счётчик ходов, сообщение о победе и кнопка «Новая игра».
- Работает офлайн, ничего не скачивает.

SHA-256 нативного EXE записан в [`MemoryGame.exe.sha256`](MemoryGame.exe.sha256).

## Пересобрать нативный EXE в Linux или Windows

Нужен Python и кросс-компилятор Zig. Из корня репозитория:

```sh
python -m pip install ziglang==0.14.1
python 25.11.2026/native/build.py
```

Результат: `25.11.2026/MemoryGame.exe` и его SHA-256.
Скрипт не затрагивает исходники C# и не скачивает артефакты GitHub Actions.
Zig использует встроенные заголовки и библиотеки Windows для кросс-компиляции.

Проверка игровой логики в Linux с GCC:

```sh
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined 25.11.2026/native/test.c -o /tmp/memory-test
/tmp/memory-test
```

Автотесты проверяют состав колоды на 1000 раскладках, повторные клики,
блокировку третьей карточки, совпадения, победу и перезапуск во время показа пары.
Формат Windows PE x64 и импорты системных DLL проверены после сборки.
**Сам графический EXE в Linux-песочнице не запускался** — для проверки интерфейса нужна Windows.

## Собрать исходную C#-версию

Нужны Windows и **.NET SDK 10**:

```powershell
dotnet run --project 25.11.2026/MemoryGame.csproj
```

Один самостоятельный EXE, как в существующем GitHub Actions workflow:

```powershell
dotnet publish 25.11.2026/MemoryGame.csproj -c Release -r win-x64 --self-contained true -p:PublishSingleFile=true -p:IncludeNativeLibrariesForSelfExtract=true -p:DebugType=None -p:DebugSymbols=false -o artifacts
```

Результат: `artifacts/MemoryGame.exe`. Устанавливать .NET игроку не нужно:
среда включена в EXE, поэтому C#-сборка значительно больше нативной.
В GitHub Actions её можно скачать из успешного запуска **Build PC game → Artifacts → MemoryGame-Windows-x64**.

## Быстрая ручная проверка интерфейса на Windows

- Нажми одну карточку дважды — ход не засчитывается.
- Открой разные карточки и быстро нажми третью — она не открывается до закрытия пары.
- Нажми «Новая игра», пока пара ждёт закрытия — новая партия остаётся корректной.
- Собери 8 пар — появляется сообщение о победе; найденные пары нельзя открыть повторно.
- Измени размер окна — поле остаётся 4×4.
