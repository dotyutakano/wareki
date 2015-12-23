/*
 WAREKI
 Displey Japanese Year calendar 
 
 ahiru studio
 
 2014/01/25 Ver.1.0.0
 2014/10/02 Ver.1.1  10gatsu -> 0gatsu :  Bug
 2014/10/02 Ver.1.2  SDK 2.6.1 
 2015/03/01 Ver.1.3  SDK 2 add YAMA no Hi at 2016/08/11 
 2015/03/09 Ver.2.0  SDK 3 
 2015/11/14 Ver.2.1  SDK 3
 2015/12/17 Ver.2.2  SDK 3.8 Battery state , Round (APLITE FW3.8)

 カラーと白黒では画像の載せ方が違うから別にした方がいいかな？
 -> 2015/12 Pebble ClassicもFW3.8になったので同じになった
 
 */

// Standard includes
#include <pebble.h>
#include "wareki.h"
#include "holiday.h"

static bool mydebug = false ; /* true | false */ 

#ifdef PBL_ROUND  
// Pebble Time Round ( 180x180 )用の座標系        
static int   x1 = 26 + 4;
static int   x2 = 26 + 26;
static int   x3 = 26 + 32;
static int   x4 = 26 + 26;
static int   y = 16 + 3;  
#else
// Pebble Time ( 144x160 )用の座標系        
static int   x1 = 26;
static int   x2 = 26;
static int   x3 = 26;
static int   x4 = 26;
static int   y = 16;
#endif

// Background color : GColorCeleste / GColorWhite
// Time Bar color   : GColorOxfordBlue / GColorBlack

// バッテリー表示の無駄な更新を減らすためのフラグ
// 10:充電中 0-9:バッテリー残量 初期値9
static int   old_battery_status = 9;

//########################################
//# destroy date layer
//# 平成27年
//# 11月14日
//# 土曜日
//########################################
static void destroy_date_layers(){
  layer_remove_from_parent(bitmap_layer_get_layer(year10_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(year01_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(month10_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(month01_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(day10_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(day01_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(wday_layer));
  bitmap_layer_destroy(year10_layer);
  bitmap_layer_destroy(year01_layer);
  bitmap_layer_destroy(month10_layer);
  bitmap_layer_destroy(month01_layer);
  bitmap_layer_destroy(day10_layer);
  bitmap_layer_destroy(day01_layer);
  bitmap_layer_destroy(wday_layer);
  gbitmap_destroy(year10_image);
  gbitmap_destroy(year01_image);
  gbitmap_destroy(month10_image);
  gbitmap_destroy(month01_image);
  gbitmap_destroy(day10_image);
  gbitmap_destroy(day01_image);
  gbitmap_destroy(wday_image);
}
//########################################
//# destroy time layer
//# 20:45
//########################################
static void destroy_time_layers(){
  layer_remove_from_parent(bitmap_layer_get_layer(HH10_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(HH01_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(MM10_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(MM01_layer));
  bitmap_layer_destroy(HH10_layer);
  bitmap_layer_destroy(HH01_layer);
  bitmap_layer_destroy(MM10_layer);
  bitmap_layer_destroy(MM01_layer);
  gbitmap_destroy(HH10_image);
  gbitmap_destroy(HH01_image);
  gbitmap_destroy(MM10_image);
  gbitmap_destroy(MM01_image);
}
//########################################
//# create date layer
//########################################
static void create_date_layers(){
  year10_layer  = bitmap_layer_create(GRect(60+x1,  0+y,14,22));
  year01_layer  = bitmap_layer_create(GRect(76+x1,  0+y,14,22));
  month10_layer = bitmap_layer_create(GRect( 3+x2, 30+y,14,22));
  month01_layer = bitmap_layer_create(GRect(19+x2, 30+y,14,22));
  day10_layer   = bitmap_layer_create(GRect(60+x2, 30+y,14,22));
  day01_layer   = bitmap_layer_create(GRect(76+x2, 30+y,14,22));
  wday_layer    = bitmap_layer_create(GRect(40+x3, 60+y,70,22));
  bitmap_layer_set_bitmap(year10_layer,  year10_image);
  bitmap_layer_set_bitmap(year01_layer,  year01_image);
  bitmap_layer_set_bitmap(month10_layer, month10_image);
  bitmap_layer_set_bitmap(month01_layer, month01_image);
  bitmap_layer_set_bitmap(day10_layer,   day10_image);
  bitmap_layer_set_bitmap(day01_layer,   day01_image);
  bitmap_layer_set_bitmap(wday_layer,    wday_image);
  bitmap_layer_set_alignment(year10_layer,GAlignLeft);
  bitmap_layer_set_alignment(year01_layer,GAlignLeft);
  bitmap_layer_set_alignment(month10_layer,GAlignLeft);
  bitmap_layer_set_alignment(month01_layer,GAlignLeft);
  bitmap_layer_set_alignment(day10_layer,GAlignLeft);
  bitmap_layer_set_alignment(day01_layer,GAlignLeft);
  bitmap_layer_set_alignment(wday_layer,GAlignLeft);
  bitmap_layer_set_compositing_mode(year10_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(year01_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(month10_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(month01_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(day10_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(day01_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(wday_layer, GCompOpSet);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(year10_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(year01_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(month10_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(month01_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(day10_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(day01_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(wday_layer));
}
//########################################
//# create time layer
//########################################
static void create_time_layers(){
  HH10_layer    = bitmap_layer_create(GRect(42+x4,90+y,14,22));
  HH01_layer    = bitmap_layer_create(GRect(58+x4,90+y,14,22));
  MM10_layer    = bitmap_layer_create(GRect(82+x4,90+y,14,22));
  MM01_layer    = bitmap_layer_create(GRect(98+x4,90+y,14,22));
  bitmap_layer_set_bitmap(HH10_layer, HH10_image);
  bitmap_layer_set_bitmap(HH01_layer, HH01_image);
  bitmap_layer_set_bitmap(MM10_layer, MM10_image);
  bitmap_layer_set_bitmap(MM01_layer, MM01_image);
  bitmap_layer_set_alignment(HH10_layer,GAlignLeft);
  bitmap_layer_set_alignment(HH01_layer,GAlignLeft);
  bitmap_layer_set_alignment(MM10_layer,GAlignLeft);
  bitmap_layer_set_alignment(MM01_layer,GAlignLeft);
  bitmap_layer_set_compositing_mode(HH10_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(HH01_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(MM10_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(MM01_layer, GCompOpSet);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(HH10_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(HH01_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(MM10_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(MM01_layer));
}

//########################################
//# handle_battery バッテリーステイタスの表示
//########################################
static void create_battery_layer(){
  battery_layer  = bitmap_layer_create(GRect(120,3,16,8));
  #ifdef PBL_ROUND    
  battery_layer  = bitmap_layer_create(GRect(82,3,16,8));
  #endif  
  layer_set_hidden(bitmap_layer_get_layer(battery_layer),false);
  bitmap_layer_set_bitmap(battery_layer, battery_image);
  bitmap_layer_set_alignment(battery_layer,GAlignLeft);
  bitmap_layer_set_compositing_mode(battery_layer, GCompOpSet);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(battery_layer));
}

static void destroy_battery_layer(){
  layer_remove_from_parent(bitmap_layer_get_layer(battery_layer));
  bitmap_layer_destroy(battery_layer);
  gbitmap_destroy(battery_image);    
}

static void handle_battery(BatteryChargeState charge_state) {
  // old_battery_status (min:0 - max:9,chg:10) との違いをチェック
  // 変化があったら書き換える(毎回書き換えしないように)
  int s = 0;
  int b50 = 5 ;  // 50%
  int b30 = 3 ;  // 30% 
  if (mydebug) {
   b50 = 9 ;
   b30 = 7 ;
  }
  
  if (charge_state.is_charging) {              // charging...
    s = 10;
    } else {
    s = ( charge_state.charge_percent - 1 ) / 10 ;
  }
  
  if ( s == old_battery_status ) {
    ; // 画面書き換え無し
  } else {
    // 画面書き換え有り
    destroy_battery_layer();
    
    battery_image   = gbitmap_create_with_resource(RESOURCE_ID_NULL);    
    if ( s < b50 ){ // 49%
      battery_image   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_50);
    }
    if ( s < b30 ){ // 29%
      battery_image   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_30);
    }
    if ( s == 10 ){ // charging
      battery_image   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_CHARGING);
    }
    
    create_battery_layer();
  } 
  old_battery_status = s; // deinit... 
}

//########################################
//# displey_holiday 祝日の表示
//########################################
static void display_holiday(struct tm *tick_time) {
  uint32_t today8 ;
  // today を 8 文字で表示  ex: 20141224
  today8 = ((tick_time->tm_year) + 1900) *10000 
         + ((tick_time->tm_mon) + 1 )*100 
         + (tick_time->tm_mday); 

  // まずはレイヤーを隠す
  layer_set_hidden(bitmap_layer_get_layer(holidayN_layer),true); 
  layer_set_hidden(bitmap_layer_get_layer(holidayF_layer),true); 

  // debug
  if (mydebug) { 
    today8 = 20250101;
  }  

  uint32_t holidaylist_Num = sizeof holidaylist /sizeof holidaylist[0];  // 配列の要素数
  uint32_t holidayImage_Num = sizeof HOLIDAY_IMG_IDS /sizeof HOLIDAY_IMG_IDS[0];  // 配列の要素数

  for( uint32_t i = 0; i < holidaylist_Num; i++ )  // リストの数だけまわす
  {
    if ( today8 == ( holidaylist[i] / 100 ) )  {
      layer_set_hidden(bitmap_layer_get_layer(holidayN_layer),false); 
      if (( holidaylist[i] % 100 ) < holidayImage_Num)  // 正常な範囲内のとき
      {
        holidayN_image  = gbitmap_create_with_resource(HOLIDAY_IMG_IDS[( holidaylist[i] % 100 )]);
        bitmap_layer_set_bitmap(holidayN_layer, holidayN_image);
        bitmap_layer_set_compositing_mode(holidayN_layer, GCompOpSet);
        if ( ( ( holidaylist[i] % 100 ) == 0 ) || ( ( holidaylist[i] % 100 ) == 16 ) ) {
          layer_set_hidden(bitmap_layer_get_layer(holidayF_layer),true);
        }else{
          layer_set_hidden(bitmap_layer_get_layer(holidayF_layer),false);
        }; 
      }else{  // データを作り間違った時。。。
              // 該当する祝日が無いのでとりあえず何も表示しないでおく。
         layer_set_hidden(bitmap_layer_get_layer(holidayF_layer),true);        
         layer_set_hidden(bitmap_layer_get_layer(holidayN_layer),true);
      }
      break; // リストにあったら終了。
    }
  }
}
//########################################
//# displey_date 
//########################################
static void display_date(struct tm *tick_time) {
  // Sat Jan 18 , 2014  (footer)
  strftime(date_text, sizeof(date_text), "%a %b %d , %Y ", tick_time);
  text_layer_set_text(date_layer,date_text);

  destroy_date_layers();

  year10_image   = gbitmap_create_with_resource(NUM_IMG_IDS[((tick_time->tm_year) - 88) / 10]); 
  year01_image   = gbitmap_create_with_resource(NUM_IMG_IDS[((tick_time->tm_year) - 88) % 10]); 
  month10_image  = gbitmap_create_with_resource(NUM_IMG_IDS[((tick_time->tm_mon)  + 1 ) / 10]); 
  month01_image  = gbitmap_create_with_resource(NUM_IMG_IDS[((tick_time->tm_mon)  + 1 ) % 10]); 
  day10_image    = gbitmap_create_with_resource(NUM_IMG_IDS[ (tick_time->tm_mday)       / 10]); 
  day01_image    = gbitmap_create_with_resource(NUM_IMG_IDS[ (tick_time->tm_mday)       % 10]); 
  wday_image     = gbitmap_create_with_resource(WDAY_IMG_IDS[tick_time->tm_wday]); 
  if ((tick_time->tm_mon + 1 ) < 10){
    month10_image  = gbitmap_create_with_resource(RESOURCE_ID_NULL); 
  }
  if  (tick_time->tm_mday      < 10){
    day10_image    = gbitmap_create_with_resource(RESOURCE_ID_NULL); 
  }

  create_date_layers();
}

//########################################
//# displey_time 
//# 画像で文字を書く場合は一度レイヤーを削除して作り直す。
//########################################
static void display_time(struct tm *tick_time) {
  // Time 
  destroy_time_layers();

  // set images
  if (clock_is_24h_style()) {
    // 24 
    HH10_image     = gbitmap_create_with_resource(NUM_IMG_IDS[(tick_time->tm_hour) / 10]); 
    HH01_image     = gbitmap_create_with_resource(NUM_IMG_IDS[(tick_time->tm_hour) % 10]); 
    strftime(dbg_text, sizeof(dbg_text), "%H:%M:%S", tick_time); // for Debug
  }else{
    // 12
    int t;
    t = (tick_time->tm_hour) % 12;
    if ( t == 0 ){
      t = 12;
    }
    if ( t > 9 ){
      HH10_image     = gbitmap_create_with_resource(NUM_IMG_IDS[1]);
    } else {
      HH10_image     = gbitmap_create_with_resource(RESOURCE_ID_NULL);
    } 
    HH01_image     = gbitmap_create_with_resource(NUM_IMG_IDS[t % 10]); 
    strftime(dbg_text, sizeof(dbg_text), "%I:%M:%S", tick_time); // for Debug
  }
  //
  MM10_image     = gbitmap_create_with_resource(NUM_IMG_IDS[(tick_time->tm_min)  / 10]); 
  MM01_image     = gbitmap_create_with_resource(NUM_IMG_IDS[(tick_time->tm_min)  % 10]); 

  snprintf(dbg_buffer, 40, "%s %d0%%", dbg_text,old_battery_status); // debug
  text_layer_set_text(dbg_layer,dbg_buffer); // debug

  create_time_layers();
  
}

//########################################
//# handle_minute_tick ( Called once per minute ) 
//########################################
static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // time
  display_time(tick_time);
  
  // battery status update
  handle_battery(battery_state_service_peek());

  // calendar update ( per day )
  if( (tick_time->tm_hour == 0 ) && (tick_time->tm_min == 0 ) ) {
    display_date(tick_time);
    display_holiday(tick_time);
  }
}

//########################################
//# do_init  初期化
//# GColorCeleste = 薄い青
//# GColorDukeBlue  = 濃い青 GColorOxfordBlue 
//# GColorPastelYellow = 薄い黄色
//# GColorArmyGreen = 濃いグリーン
//########################################
static void do_init() {
	// --------------------------------------------------------
	// Main Window
	// メインのウィンドウ
	// --------------------------------------------------------
	window = window_create();
	window_stack_push(window, true);
	window_set_background_color(window, GColorWhite);   
	#ifdef PBL_COLOR
	window_set_background_color(window, GColorCeleste  );
	#endif

	// --------------------------------------------------------
	// デバッグ用の文字をクリアで書く。
	// デバッグの時は黒にすると 
	// --------------------------------------------------------
	#ifdef PBL_ROUND  
	dbg_layer = text_layer_create(GRect(12,125,140,14));
	#else
	dbg_layer = text_layer_create(GRect(2,125,140,14));
	#endif
  text_layer_set_font(dbg_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(dbg_layer));
  text_layer_set_text_color(dbg_layer, GColorWhite);
  text_layer_set_background_color(dbg_layer, GColorWhite);
  #ifdef PBL_COLOR
  text_layer_set_text_color(dbg_layer, GColorCeleste);
  text_layer_set_background_color(dbg_layer, GColorCeleste);
  #endif
  layer_set_hidden(text_layer_get_layer(dbg_layer),true);
  	if (mydebug) { // GColorBlack
    	text_layer_set_text_color(dbg_layer, GColorBlack);
    	layer_set_hidden(text_layer_get_layer(dbg_layer),false);
  	}

  	// --------------------------------------------------------
  	// date_layer Footer 一番下に表示する日付バー
  	// --------------------------------------------------------  
  	#ifdef PBL_ROUND  
  	date_layer = text_layer_create(GRect(0,138,180,42));
  	#else
  	date_layer = text_layer_create(GRect(0,144,144,24));
  	#endif
  text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));
  text_layer_set_text_alignment(date_layer,GTextAlignmentCenter);
  text_layer_set_text_color(date_layer, GColorWhite);
  text_layer_set_background_color(date_layer, GColorBlack);
  #ifdef PBL_COLOR
  text_layer_set_text_color(date_layer, GColorCeleste );
  text_layer_set_background_color(date_layer, GColorOxfordBlue );
  #endif
	
  	// --------------------------------------------------------
  	// 時刻 now 
  	// --------------------------------------------------------
  	time_t now = time(NULL);
  	struct tm *tick_time = localtime(&now);

  // --------------------------------------------------------
  // 表示の初期化 init 
  // --------------------------------------------------------
  // 時刻などは仮のもの入れて初期化
  battery_image  = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_CHARGING);
  heisei_image   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HEISEI);
  year_image     = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_YEAR); 
  year10_image   = gbitmap_create_with_resource(NUM_IMG_IDS[1]); 
  year01_image   = gbitmap_create_with_resource(NUM_IMG_IDS[2]); 
  month_image    = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MONTH); 
  month10_image  = gbitmap_create_with_resource(NUM_IMG_IDS[3]); 
  month01_image  = gbitmap_create_with_resource(NUM_IMG_IDS[4]); 
  day_image      = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DAY); 
  day10_image    = gbitmap_create_with_resource(NUM_IMG_IDS[5]); 
  day01_image    = gbitmap_create_with_resource(NUM_IMG_IDS[6]); 
  wday_image     = gbitmap_create_with_resource(WDAY_IMG_IDS[0]); 
  HH10_image     = gbitmap_create_with_resource(NUM_IMG_IDS[7]); 
  HH01_image     = gbitmap_create_with_resource(NUM_IMG_IDS[8]); 
  colon_image    = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_COLON); 
  MM10_image     = gbitmap_create_with_resource(NUM_IMG_IDS[9]); 
  MM01_image     = gbitmap_create_with_resource(NUM_IMG_IDS[0]); 
  holidayN_image = gbitmap_create_with_resource(HOLIDAY_IMG_IDS[0]);
  holidayF_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY);

  // 数字以外のもの描画
  // Date [ 平成 年 月 日 ]
  heisei_layer   = bitmap_layer_create(GRect( 9+x1, 0+y,48,22));
  year_layer     = bitmap_layer_create(GRect(90+x1, 0+y,22,22));
  month_layer    = bitmap_layer_create(GRect(32+x2, 30+y,22,22));
  day_layer      = bitmap_layer_create(GRect(90+x2, 30+y,22,22));
  // Time colon [ : ] ( create_time_layer() )
  colon_layer    = bitmap_layer_create(GRect(72+x4, 90+y,8,22));
  // Holiday Flag , Holiday Name
  holidayF_layer = bitmap_layer_create(GRect(4,68+y,52,31));
  #ifdef PBL_ROUND
  holidayF_layer = bitmap_layer_create(GRect(14,68+y,52,31));
  #endif
  layer_set_hidden(bitmap_layer_get_layer(holidayF_layer),true); 
  bitmap_layer_set_background_color(holidayF_layer, GColorClear);
  bitmap_layer_set_alignment(holidayF_layer,GAlignTop);

  holidayN_layer = bitmap_layer_create(GRect(4,102+y,52,8));
  #ifdef PBL_ROUND
  holidayN_layer = bitmap_layer_create(GRect(14,102+y,52,8));
  #endif
  layer_set_hidden(bitmap_layer_get_layer(holidayN_layer),true); 
  bitmap_layer_set_background_color(holidayN_layer, GColorClear);
  bitmap_layer_set_alignment(holidayN_layer,GAlignTop);

  // bitmap のセット
  bitmap_layer_set_bitmap(heisei_layer, heisei_image);
  bitmap_layer_set_bitmap(year_layer, year_image);
  bitmap_layer_set_bitmap(month_layer, month_image);
  bitmap_layer_set_bitmap(day_layer, day_image);
  bitmap_layer_set_bitmap(colon_layer, colon_image);
  bitmap_layer_set_bitmap(holidayN_layer, holidayN_image);
  bitmap_layer_set_bitmap(holidayF_layer, holidayF_image);
  
  bitmap_layer_set_compositing_mode(heisei_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(year_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(month_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(day_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(colon_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(holidayN_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(holidayF_layer, GCompOpSet);
  
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(heisei_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(year_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(month_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(day_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(colon_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(holidayN_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(holidayF_layer));

  // 数字の部分の描画 初期化
  create_time_layers();
  create_date_layers();
  create_battery_layer();

  // 時刻日付の表示
  display_time(tick_time);
  display_date(tick_time);
  display_holiday(tick_time);

  // 画面書き換え処理 秒表示が無いなら上のでよい
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);  // 毎分0秒の処理 
  //  tick_timer_service_subscribe(SECOND_UNIT, handle_minute_tick);  // 毎秒の処理。

  // Subscribe to the Battery State Service
  // バッテリーチェック
  battery_state_service_subscribe(handle_battery);
}

//########################################
//# do_deinit 終了処理
//########################################
static void do_deinit() {
  destroy_time_layers();
  destroy_date_layers();
  destroy_battery_layer();

  layer_remove_from_parent(bitmap_layer_get_layer(colon_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(holidayF_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(holidayN_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(month_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(day_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(year_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(heisei_layer));
  gbitmap_destroy(colon_image);
  gbitmap_destroy(holidayN_image);
  gbitmap_destroy(holidayF_image);
  gbitmap_destroy(month_image);
  gbitmap_destroy(day_image);
  gbitmap_destroy(heisei_image);
  gbitmap_destroy(year_image);
  text_layer_destroy(date_layer);
  text_layer_destroy(dbg_layer);
  bitmap_layer_destroy(year_layer);
  bitmap_layer_destroy(heisei_layer);
  bitmap_layer_destroy(month_layer);
  bitmap_layer_destroy(day_layer);
  bitmap_layer_destroy(colon_layer);
  bitmap_layer_destroy(holidayN_layer);
  bitmap_layer_destroy(holidayF_layer);

  window_destroy(window);
}
//########################################
//# main 
//########################################
int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
//########################################
//# END
//########################################