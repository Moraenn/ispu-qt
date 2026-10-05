# Отчет по лабораторной работе

## 1. Задание

Тема: изучение технологии `model-view` и создание собственных моделей данных в Qt.

Необходимо разработать приложение `Справочник филателиста`, предоставляющее сведения о коллекции почтовых марок:

- название марки;
- страна;
- год выпуска;
- тираж.

Приложение должно обеспечивать добавление, удаление и редактирование записей в режиме диалога с пользователем. Для доступа к данным требуется разработать собственную модель, для отображения данных создать представление, а для редактирования использовать делегат. Пользовательский интерфейс должен быть построен с применением менеджеров компоновок.

## 2. Диаграмма классов приложения

Диаграмма классов сохранена в файле [class_diagram.puml](/Users/xmoraennx/ispu/qt/handbook/class_diagram.puml).

Основные классы приложения:

- `Stamp` хранит данные одной марки.
- `StampModel` наследуется от `QAbstractTableModel` и предоставляет данные представлению.
- `StampTableView` наследуется от `QTableView` и отвечает за отображение коллекции.
- `StampDelegate` наследуется от `QStyledItemDelegate` и открывает диалог редактирования записи.
- `StampDialog` реализует диалог добавления и изменения данных.
- `MainWindow` объединяет модель, представление, делегат и управляющие кнопки.

## 3. Листинг программы

В состав проекта входят следующие файлы:

- [handbook.pro](/Users/xmoraennx/ispu/qt/handbook/handbook.pro)
- [main.cpp](/Users/xmoraennx/ispu/qt/handbook/main.cpp)
- [mainwindow.h](/Users/xmoraennx/ispu/qt/handbook/mainwindow.h)
- [mainwindow.cpp](/Users/xmoraennx/ispu/qt/handbook/mainwindow.cpp)
- [stamp.h](/Users/xmoraennx/ispu/qt/handbook/stamp.h)
- [stampmodel.h](/Users/xmoraennx/ispu/qt/handbook/stampmodel.h)
- [stampmodel.cpp](/Users/xmoraennx/ispu/qt/handbook/stampmodel.cpp)
- [stampdialog.h](/Users/xmoraennx/ispu/qt/handbook/stampdialog.h)
- [stampdialog.cpp](/Users/xmoraennx/ispu/qt/handbook/stampdialog.cpp)
- [stampdelegate.h](/Users/xmoraennx/ispu/qt/handbook/stampdelegate.h)
- [stampdelegate.cpp](/Users/xmoraennx/ispu/qt/handbook/stampdelegate.cpp)
- [stamptableview.h](/Users/xmoraennx/ispu/qt/handbook/stamptableview.h)
- [stamptableview.cpp](/Users/xmoraennx/ispu/qt/handbook/stamptableview.cpp)

При оформлении отчета полный листинг следует вставить после данного раздела или вынести в приложение.

## 4. Скриншоты работы приложения

В отчет необходимо вставить:

- главный экран приложения с таблицей марок;
- диалог добавления новой марки;
- диалог редактирования существующей записи;
- пример удаления записи;
- скриншот запуска под Windows;
- скриншот запуска под Ubuntu Linux.

Примечание: в текущем окружении выполнена сборка и проверка запуска на локальной Qt-среде, но скриншоты под Windows и Ubuntu требуется сделать вручную на соответствующих ОС.

## 5. Выводы по работе

В ходе выполнения работы было разработано приложение `Справочник филателиста` на Qt, использующее архитектуру `model-view`. Для хранения и предоставления данных создан собственный класс модели `StampModel`, наследованный от `QAbstractTableModel`. Для отображения коллекции марок реализовано собственное представление `StampTableView`, а для редактирования записей через модальный диалог использован делегат `StampDelegate`.

В результате были закреплены навыки:

- разработки пользовательских моделей данных в Qt;
- использования `QTableView` для табличного представления информации;
- применения делегатов для редактирования данных;
- построения интерфейса с помощью менеджеров компоновок;
- организации взаимодействия между моделью, представлением и диалоговыми окнами.

## 6. Список использованных источников

1. Документация Qt Framework. Модуль `Qt Widgets`.
2. Документация Qt Framework. Класс `QAbstractTableModel`.
3. Документация Qt Framework. Класс `QTableView`.
4. Документация Qt Framework. Класс `QStyledItemDelegate`.
5. Документация Qt Framework. Класс `QDialog`.
