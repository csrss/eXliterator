#ifndef _LANG_H
#define _LANG_H
/******************************************************************/
#define ENGLISH 0
#define POLISH	1
#define RUSSIAN	2
/******************************************************************
				MAIN APPLICATION WINDOW LANGUAGE
******************************************************************/
//1
wchar_t *translit_area_tool[] ={L"Applications windows for transliteration are gather here",
								L"Zebrane są tu okna aplikacji dla transliteracji",
								L"Собраны тут все окна других программ для транслита"};
//2
wchar_t *translation_area_tool[] = {L"All translations are gathered here", 
									L"Zebrane są tu wszystkie wykonane tłumaczenia", 
									L"Все переводы собраны тут"};
//3
wchar_t *main_radio_tool[] = {	L"Turn transliteration ON or OFF", 
								L"Włączyć albo wyłączyć transliterację", 
								L"Включить или выключить транслит"};
//4
wchar_t *translation_dirsel_tool[] = {	L"Type of dictionary selection", 
										L"Wybór rodzaju słownika", 
										L"Тип словаря"};
//5
wchar_t *word_entry_tool[] = {	L"Entry for a word that will be translated", 
								L"Pole dla wyrazu który będzie przetłumaczony na inny język", 
								L"Поле ввода для слова, которое будет переведено на другой язык"};
//6
wchar_t *fetch_button_tool[] = {L"Search for a translation", 
								L"Szukać tłumaczenia wyrazu", 
								L"Искать перевод слова"};
//7
wchar_t *dictdb_button_tool[] ={L"Show dictionary database window", 
								L"Pokazać okno bazy danych słownika", 
								L"Показать окно словарной базы даных"};
//8
wchar_t *settings_button_tool[] = {	L"Show settings window", 
									L"Pokazać okno z ustawieniami", 
									L"Показать окно с опциями программы"};
//9
wchar_t *quit_button_tool[] = {	L"Quit application", 
								L"Zakończyć program", 
								L"Завершить программу"};
/******************************************************************************/
wchar_t *translate_frame_name[] = {	L"Translation", 
									L"Tłumaczenie", 
									L"Перевод"};

wchar_t *fetch_button_name[] = {L"Translate", 
								L"Tłumacz", 
								L"Перевод"};

wchar_t *dictdb_button_name[] ={L"Dict DB", 
								L"Słownik", 
								L"Словарь"};

wchar_t *settings_button_name[] = {	L"Settings", 
									L"Opcje", 
									L"Установки"};

wchar_t *quit_button_name[] = {	L"Quit", 
								L"Zakończ", 
								L"Выход"};
/******************************************************************************/
wchar_t *dictype_en_pl[] = {L" English » Polish", 
							L" Angielski » Polski", 
							L" Английский » Польский"};

wchar_t *dictype_en_ru[] = {L" English » Russian", 
							L" Angielski » Rosyjski", 
							L" Английский » Русский"};

wchar_t *dictype_pl_en[] = {L" Polish » English", 
							L" Polski » Angielski", 
							L" Польский » Английский"};

wchar_t *dictype_pl_ru[] = {L" Polish » Russian", 
							L" Polski » Rosyjski", 
							L" Польский » Русский"};

wchar_t *dictype_ru_en[] = {L" Russian » English", 
							L" Rosyjski » Angielski", 
							L" Русский » Английский"};

wchar_t *dictype_ru_pl[] = {L" Russian » Polish", 
							L" Rosyjski » Polski", 
							L" Русский » Польский"};
/*************************************************************************
						SETTINGS WINDOW LANGUAGE
*************************************************************************/
wchar_t *app_lang_style_group[] = {	L"Application language and style", 
									L"Styl okna programu oraz język", 
									L"Стиль окна программы и язык"};

wchar_t *app_settings_group[] ={L"Application settings", 
								L"Ustawienia programu", 
								L"Опции программы"};

wchar_t *dict_time_search[] = {	L"Dictionary timeout and search options", 
								L"Time-out dla wyszukiwania tłumaczenia w słownikach online", 
								L"Тайм-аут для поиска перевода он-лайн"};

wchar_t *start_with_system[] = {L"Start with system", 
								L"Uruchomienie z systemem",
								L"Автостарт"};

wchar_t *manual_start[] = {	L"Check for updates", 
							L"Sprawdzać czy są aktualizacje", 
							L"Проверка обновлений"};

wchar_t *debug_log[] = {L"Debug log", 
						L"Logowanie", 
						L"Логирование"};

wchar_t *global_russian[] = {	L"Global transliteration", 
								L"Globalna transliteracja", 
								L"Глобальный транслит"};

wchar_t *local_search[] = {	L"Local search", 
							L"Lokalne wyszukiwanie", 
							L"Локальный поиск"};

wchar_t *online_search[] = {L"Online search", 
							L"Wyszukiwanie online", 
							L"Он-лайн поиск"};

wchar_t *close_button[] = {	L"Close", 
							L"Zamknąć", 
							L"Закрыть"};

wchar_t *activate_button[] = {	L"Activate", 
								L"Aktywacja", 
								L"Активация"};

wchar_t *register_button[] = {	L"Register", 
								L"Rejestracja", 
								L"Регистрация"};

wchar_t *show_tooltips[] = {L"Show Tootips", 
							L"Pokazywać podpowiedzi", 
							L"Показывать подсказки"};

wchar_t *show_col_headers[] = {	L"Show Columns Headers", 
								L"Nagłówki tabel", 
								L"Верхние колонтитулы"};

wchar_t *bottom_glow[] = {	L"Bottom glow text", 
							L"Pokazać dolny tekst", 
							L"Текст внизу программы"};

wchar_t *buttons_for_icons[] = {L"Buttons icons", 
								L"Ikony na przyciskach", 
								L"Иконки на кнопках"};

wchar_t *app_status_group[] = {	L"Activation Status", 
								L"Status aplikacji", 
								L"Статус программы"};

wchar_t *mflogin_group[] = {L"Your Machinized Login", 
							L"Twój login na Machinized Fractals", 
							L"Твой логин на Machinized Fractals"};
/*************************************************************************
					SETTINGS WINDOW TOOLTIPS
*************************************************************************/
	//1
wchar_t *tool_lang_selection[] = {	L"Select application interface language.", 
									L"Wybór języka interfejsu graficznego.", 
									L"Выбор языка окна програмы."};
	//2
wchar_t *tool_autostart[] = {	L"Enable application autostart. It will start with system start.", 
								L"Uruchomić opcję autostartu. Aplikacja będzie uruchamiana wraz z systemem.", 
								L"Включить автозапуск. Программа будет запускаться вместе с системой."};
	//3
wchar_t *tool_manual_start[] = {L"User start application manually.", 
								L"Aplikacja manualnie uruchamiana przez użytkownika.", 
								L"Запуск программы пользователем в ручную."};
	//4
wchar_t *tool_global_ru[] ={L"While checked, turning ON transliteration will set transliterated russian for all windows.", 
							L"Zaznaczenie tej opcji spowoduje, podczas włączania trybu transliteracji, użycie języka rosyjskiego dla wszystkich okien.", 
							L"Опция предназначена для того, чтобы во время включения транслита включить русский для всех окон."};
	//5
wchar_t *tool_debug_log[] ={L"Log debug events into a file for later possible errors checking.", 
							L"Zapis zdarzeń do pliku dla późniejszej identyfikacji możliwych błędów programu.", 
							L"Логирование отладочных сообщений в файл для позднейшего анализа возможных ошибок программы."};
	//6
wchar_t *tool_local_search[] = {L"Enable local search. That means, an application will search for translation in local database.", 
								L"Włączyć funkcję wyszukiwania tłumaczenia w lokalnej bazie.", 
								L"Включить поиск перевода в локальной базе."};
	//7
wchar_t *tool_net_search[] = {	L"Enable online search. That means, an application will search for translation in online dictionary service.", 
								L"Włączyć wyszukiwanie tłumaczenia w słowniku online.", 
								L"Включить поиск перевода в он-лайн словаре."};
	//8
wchar_t *tool_conn_timeout[] = {L"Connection time out while fetching translation from on-line dictionary.", 
								L"Time-out dla połączenia ze słownikiem on-line.", 
								L"Тайм аут для соединения с он-лайн словарём."};
	//9
wchar_t *tool_visual_style[] = {L"Select application visual style. Standard or Vista glass.", 
								L"Wybór stylu okna programu. Standardowy albo Vista glass.", 
								L"Стиль окна программы. Стандартный или Vista glass."};

	//10
wchar_t *bottom_text_tool[] = {	L"Glowing text on a very bottom of an application. Turning it will result in speeding up eXsliterator.", 
								L"Świecący tekst na samym dole aplikacji. Wyłączenie go spowoduje przyśpieszenie działania aplikacji.", 
								L"Светящийся текст в самом низу программы. Если его выключить, то прога будет работать быстрее."};
	//11
wchar_t *buttons_icons_tool[] ={L"Display icons on buttons. Turning it off will result in nothing.", 
								L"Wyświetlanie ikonek na przyciskach.", 
								L"Показывать иконки на кнопках."};
	//12
wchar_t *show_tootips_tool[]={	L"Show tootips with helper text when mouse hoovers controls.", 
								L"Pokazywać tekst podpowiedzi po najechaniu myszką.", 
								L"Показывать текст с подсказкой ."};
	//13
wchar_t *app_activate_tool[] = {L"Is your copy of eXsliterator activated?", 
								L"Czy ta kopia programu została aktywowana?", 
								L"Активирована ли программа?"};
	//14
wchar_t *mflogin_tool[] = {	L"Your login at Machinized Fractals website.", 
							L"Twój login na stronie Machinized Fractals.", 
							L"Твой логин на сайте Machinized Fractals."};
	//15
wchar_t *activate_button_tool[] = {	L"This will check if your copy can be activated on remote server.", 
									L"Sprawdzenie, czy ta kopia programu może zostać aktywowana.", 
									L"Проверка на счёт возможности активации программы."};
	//16
wchar_t *register_button_tool[] = {	L"Will point you to account registration.", 
									L"Rejestracja konta na Machinized Fractals.", 
									L"Регистрация аккаунта на Machinized Fractals."};
	//17
wchar_t *close_button_tool[] = {L"Close this window.", 
								L"Zamknąć okno ustawień.", 
								L"Закрыть окно настроек."};
	//18
wchar_t *show_col_headers_tool[] = {L"Show columns headers.", 
									L"Pokazywać nagłówki tabel, tzn. nagłówki kolumn w tabelach.", 
									L"Показывать заголовки колумн в таблицах."};
	//19
wchar_t *top_grid_bkg_tool[] = {L"Transliterated Table background image.", 
								L"Tło górnej tabeli.", 
								L"Фон верхней таблицы."};
	//20
wchar_t *bot_grid_bkg_tool[] = {L"Translations Table background image.", 
								L"Tło dolnej tabeli.", 
								L"Фон нижней таблицы."};
/*************************************************************************/

int __stdcall SetOptionsText(HWND hWnd, int Lang){
	SendDlgItemMessage (hWnd, IDC_STATIC_LANGUAGE_GROUPBOX, WM_SETTEXT, 
							0, (LPARAM) app_lang_style_group[Lang]);
	SendDlgItemMessage (hWnd, IDC_STATIC_SETTINGS_GROUPBOX, WM_SETTEXT, 
							0, (LPARAM) app_settings_group[Lang]);
	SendDlgItemMessage (hWnd, IDC_STATIC_DICTIONARY_GROUPBOX, WM_SETTEXT, 
							0, (LPARAM) dict_time_search[Lang]);
	SendDlgItemMessage (hWnd, IDC_RADIO1, WM_SETTEXT, 0, (LPARAM) start_with_system[Lang]);
	SendDlgItemMessage (hWnd, IDC_RADIO2, WM_SETTEXT, 0, (LPARAM) manual_start[Lang]);
	SendDlgItemMessage (hWnd, IDC_CHECK1, WM_SETTEXT, 0, (LPARAM) debug_log[Lang]);
	SendDlgItemMessage (hWnd, IDC_CHECK2, WM_SETTEXT, 0, (LPARAM) global_russian[Lang]);
	SendDlgItemMessage (hWnd, IDC_CHECK3, WM_SETTEXT, 0, (LPARAM) local_search[Lang]);
	SendDlgItemMessage (hWnd, IDC_CHECK4, WM_SETTEXT, 0, (LPARAM) online_search[Lang]);
	SendDlgItemMessage (hWnd, IDOK, WM_SETTEXT, 0, (LPARAM) close_button[Lang]);
	SendDlgItemMessage (hWnd, IDC_CHECK5, WM_SETTEXT, 0, (LPARAM) bottom_glow[Lang]);
	SendDlgItemMessage (hWnd, IDC_CHECK6, WM_SETTEXT, 0, (LPARAM) buttons_for_icons[Lang]);
	SendDlgItemMessage (hWnd, IDC_BUTTON1, WM_SETTEXT, 0, (LPARAM) register_button[Lang]);
	SendDlgItemMessage (hWnd, IDC_BUTTON2, WM_SETTEXT, 0, (LPARAM) activate_button[Lang]);
	SendDlgItemMessage (hWnd, IDC_CHECK7, WM_SETTEXT, 
							0, (LPARAM) show_tooltips[Lang]);
	SendDlgItemMessage (hWnd, IDC_CHECK8, WM_SETTEXT, 
							0, (LPARAM) show_col_headers[Lang]);
	SendDlgItemMessage (hWnd, IDC_STATIC_ACTIVATION_GROUP, WM_SETTEXT, 
							0, (LPARAM) app_status_group[Lang]);
	SendDlgItemMessage (hWnd, IDC_STATIC_MFLOGIN_GROUP, WM_SETTEXT, 
							0, (LPARAM) mflogin_group[Lang]);

	return 0;
}

/*************************************************************************/

wchar_t *cntx_menu_reload[] = { L"Reload List", 
								L"Odświeżyć listę", 
								L"Обновить список"};

wchar_t *cntx_menu_clear[] = {	L"Clear List", 
								L"Oczyścić listę", 
								L"Очистить список"};

wchar_t *cntx_menu_remove[] = {	L"Permanently Remove From List", 
								L"Permanentnie Usunąć z listy", 
								L"Удалить из списка на совсем"};

wchar_t *cntx_menu_settr[] = {	L"Set Transliteration", 
								L"Włączyć transliterację dla tego okna", 
								L"Включить транслит для этого окна"};

wchar_t *cntx_menu_unsettr[] = {L"Remove Transliteration", 
								L"Wyłączyć transliterację dla tego okna", 
								L"Выключить транслит для этого окна"};

/*************************************************************************
					DICTIONARY LANGUAGE SETTINGS
*************************************************************************/

wchar_t *dict_close_btn[] ={L"   Close", 
							L"   Zamknąć", 
							L"   Закрыть"};

wchar_t *dict_delete_btn[]={L"   Delete", 
							L"   Usunąć", 
							L"   Удалить"};

wchar_t *dict_update_btn[]={L"   Save", 
							L"   Zapisać", 
							L"   Записать"};

wchar_t *dict_insert_btn[]={L"   Insert", 
							L"   Dodać", 
							L"   Добавить"};

wchar_t *dict_search_btn[]={L"   Search", 
							L"   Szukać", 
							L"   Искать"}; 

wchar_t *dict_words_header[] = {L"Words List", 
								L"Lista Wyrazów", 
								L"Список Слов"};

int __stdcall SetDictionaryText(HWND hWnd, int Lang){
	SendDlgItemMessage (hWnd, IDCANCEL, WM_SETTEXT, 
							0, (LPARAM) dict_close_btn[Lang]);
	SendDlgItemMessage (hWnd, IDC_BUTTON2, WM_SETTEXT, 
							0, (LPARAM) dict_delete_btn[Lang]);
	SendDlgItemMessage (hWnd, IDC_BUTTON1, WM_SETTEXT, 
							0, (LPARAM) dict_update_btn[Lang]);
	SendDlgItemMessage (hWnd, IDOK, WM_SETTEXT, 
							0, (LPARAM) dict_insert_btn[Lang]);
	SendDlgItemMessage (hWnd, IDC_BUTTON3, WM_SETTEXT, 
							0, (LPARAM) dict_search_btn[Lang]);
	return 0;
}

wchar_t *dict_tool_search[] = {	L"Entry field for a word that will be added to a dictionary or for which translation will be searched.", 
								L"Pole dla wyrazu, który zostanie dodany do słownika albo też będzie dla niego wyszukiwane tłumaczenie.", 
								L"Поле ввода для слова, которое будет либо добавлено в словарь ливо для которого будет осуществлятся поиск перевода."};

wchar_t *dict_tool_dicttype[] ={L"Dictionary type, in a context of which we are currently working.", 
								L"Rodzaj słownika, w kontekście którego pracujemy na daną chwilę.", 
								L"Тип словаря, в контексте которого работаем на данный момент."};

wchar_t *dict_tool_searchbtn[]={L"Search for a translation.", 
								L"Szukać tłumaczenie wyrazu.", 
								L"Искать перевод для слова."};

wchar_t *dict_tool_wordslist[]={L"Words list from a current dictionary.", 
								L"Lista wyrazów z obecnie używanego słownika.", 
								L"Список слов из используемого на данный момент словаря."};

wchar_t *dict_tool_translation[] = {L"Window for a translation result.", 
									L"Okno dla wyników tłumaczenia.", 
									L"Окно для результатов перевода."};

wchar_t *dict_tool_insert[] = {	L"Insert new word with a translation into current dictionary.", 
								L"Dodać wyraz wraz z tłumaczeniem do obecnie używanego słownika.", 
								L"Добавить слово вместе с переводом в словарь, используемый на данный момент."};

wchar_t *dict_tool_save[] ={L"Update current selected translation.", 
							L"Zapisać zmiany w tłumaczeniu.", 
							L"Записать изменения."};

wchar_t *dict_tool_delete[] = {	L"Delete translation from dictionary.", 
								L"Usunąć tłumaczenie ze słownika.", 
								L"Убрать перевод из словаря."};

wchar_t *dict_tool_close[]={L"Close this window. Basically, close a dictionary window.", 
							L"Zamknąć okno słownika.", 
							L"Закрыть окно словаря."};

/*************************************************************************
					TRAY LANGUAGE SETTINGS
*************************************************************************/

wchar_t *tray_global_translit[] = {	L"Translit", 
									L"Transliteracja", 
									L"Транслит"};

wchar_t *tray_restore_window[]={L"Restore Window", 
								L"Przywrócić okno", 
								L"Показать окно"};

wchar_t *tray_quit[] = {L"Quit", 
						L"Zakończyć aplikację", 
						L"Завершить программу"};

/*************************************************************************
					GRIDS LANGUAGE SETTINGS
*************************************************************************/

wchar_t *grid_status[] = {L"Status", L"Status", L"Статус"};
wchar_t *grid_window[] = {L"Window", L"Okno", L"Окно"};
wchar_t *grid_word[] = {L"Word", L"Wyraz", L"Слово"};
wchar_t *grid_translation[] = {L"Translation", L"Tłumaczenie", L"Перевод"};

/*************************************************************************
					ACTIVATION LANGUAGE SETTINGS
*************************************************************************/

wchar_t *activ_line[] = {L"eXsliteratoЯ™ is activated", 
						 L"eXsliteratoЯ™ jest aktywowany", 
						 L"eXsliteratoЯ™ активирован"};

wchar_t *noactive_line[] = {L"eXsliteratoЯ™ is not activated", 
							L"eXsliteratoЯ™ nie został aktywowany", 
							L"eXsliteratoЯ™ не активирован"};

wchar_t *translation_type[] = { L"Dictionary...", 
								L"Słownik...", 
								L"Словарь..."};

wchar_t *no_dict_error[] = {L"Please, select a type of a dictionary!", 
							L"Proszę wybrać rodzaj słownika!", 
							L"Выберите, пожалуйста, тип словаря!"};

wchar_t *SerialEmpty[] = {	L"Activation code was an empty string!", 
							L"Kod aktywacyjny był pusty!", 
							L"Активационный код был пустой!"};

wchar_t *WsaError[] = {	L"Initializing socket error! Contact info@machinized.com for more information.", 
						L"Initializing socket error! Proszę o powiadomienie na info@machinized.com.", 
						L"Initializing socket error! Contact info@machinized.com for more information."};

wchar_t *SocketError[] = {	L"Connecting authentification server error! Contact info@machinized.com for more information.", 
							L"Połączenie z serwerem autentyfikacji nie powiodło się! Proszę o powiadomienie na info@machinized.com.", 
							L"Невозможно установить соединение с сервером авторизации! Contact info@machinized.com for more information."};

wchar_t *NotYet[] = {L"Your copy of eXsliterator has not been authentificated yet!", 
					 L"Ta kopia programu nie została jeszcze autoryzowana!", 
					 L"Эта копия пока что еще не авторизирована!"};

wchar_t *Authed[] = {L"This copy of eXsliterator has been activated!", 
					 L"Ta kopia programu została autoryzowana!", 
					 L"Эта копия авторизирована!"};
/*********************************************************************************/

wchar_t *NoDict[] = {L"Please, select dictionary type!", 
					 L"Proszę wybrać rodzaj słownika!", 
					 L"Пожалуйста, выберите тип словаря!"};

wchar_t *TranslationNotFound[] = {	L"Translation not found!", 
									L"Tłumaczenie nie zostało odnalezione!", 
									L"Перевод не найден!"};

wchar_t *WordIsThere[] = {	L"This word is already traslated and displayed.", 
							L"Ten wyraz już został przetłumaczony i jest wyświelony w tabeli poniżej.", 
							L"Это слово уже переведено и показано в таблице переводов."};

wchar_t *ConnectionTimeOut[] = {L"Connection with online dictionary timeout", 
								L"Przekroczono czas połączenia ze słownikiem online", 
								L"Тайм-аут соединения с он-лайн словарём"};

wchar_t *TranslationThRun[] = {	L"Translation search is in progress", 
								L"Wyszukiwanie tłumaczenia jest w toku", 
								L"Поиск перевода осуществляется"};

wchar_t *RegAttempt[] = {L"Your registration attempt has been recorded, wait for authorization.", 
						 L"Program został zarejestrowany, proszę czekać na autoryzację.", 
						 L"Программа зарегистрирована, авторизация наступит в течении 24 часов."};

wchar_t *NoSuchUser[] = {L"No such user!", 
						 L"Taki użytkownik nie istnieje.", 
						 L"Нет такого пользователя."};

wchar_t *NoSuchProduct[] = {L"No such product.", 
							L"Taki produkt nie istnieje.", 
							L"Этот продукт не существует."};

wchar_t *ClientNameEmpty[] = {	L"Client login name is empty!", 
								L"Login użytkownika jest puste!", 
								L"Логин пустой!"};

wchar_t *UpdateAvailable[] = {	L"Update available. Please visit www.machinized.com website for more information.", 
								L"Uaktualnienie programu jest dostępne. Proszę o odwiedzenie witryny www.machinized.com.", 
								L"Обновление для программы доступно. Больше информации на  www.machinized.com."};

wchar_t *Blocked[] = {	L"Application has been blocked!", 
						L"Aplikacja zablokowana!", 
						L"Программа заблокирована!"};

int * __stdcall NonRegisteredMessage(__in int Lang){
	wchar_t *Text[] = {L"Non-registered session is over. Consider register your copy of eXsliterator for unlimited use.", 
					   L"Sesja niezarejestrowanego użytkownika zakończyła się. Proszę o zarejestrowanie tej kopii eXsliterator'a w celu usunięcia ograniczeń.", 
					   L"Ниезарегистрированная сессия закончилась. Следует авторизировать программу для безлимитного пользования."};
	MessageBoxW(0, Text[Lang], L"INFORMATION", MB_ICONINFORMATION);
	return 0;
}

#endif