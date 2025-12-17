# Instrukcja uruchomienia projektu

## Wymagane paczki
- [ImGui](https://github.com/ocornut/imgui)
- [GLFW](https://www.glfw.org/download)
- [Glad](https://glad.dav1d.de/)
- [ImPlot](https://github.com/epezent/implot)
- [CMake](https://cmake.org/download/)
- OpenGL

## Pobieranie kodu

Otwórz terminal i wykonaj poniższą komendę
```sh
git clone https://github.com/JanTopolewski/ball-trajectory-sim.git
```

zbuduj projekt dla Visual Studio przy użyciu CMake i zapisz go w katalogu /build

otwórz katalog /build, następnie otwórz plik .sln w Visual Studio 

## Konfiguracja bibliotek do wyświetlania głownego okienka

Aby skonfigurować GLFW, Glad, OpenGL użyj [instrukcji](https://learnopengl.com/Getting-started/Creating-a-window)

## Instalacja Imgui

Pobierz [ImGui](https://github.com/ocornut/imgui) i wypakuj z zipa

Aby połączyć ImGui z Visual Studio użyj [tutoriala](https://www.youtube.com/watch?v=VRwhNKoxUtk)

## Instalacja ImPlot

Pobierz [ImPlot](https://github.com/epezent/implot) i wypakuj do z zipa

Dalej zrób tak samo jak z folderem ImGui:

- dodaj nowe filtry nazawne `implot` do `Header Files` i `Source Files` w projekcie TrajectorySimulation
- do filtru w `Header Files` dodaj rozpakowane pliki `.h` z folderu implot
- do filtru w `Source Files` dodaj rozpakowane pliki `.cpp` z folderu implot