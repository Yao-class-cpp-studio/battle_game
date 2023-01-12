# Core

杩欎釜鏂囦欢澶逛笅鐨勫唴瀹规槸鏁翠釜椤圭洰鐨勬牳蹇冩鏋躲€�
[GameCore](game_core.h) 绫绘槸鎵€鏈夋父鎴忛€昏緫闆嗕腑鍗忚皟鐨勫湴鏂广€�

鍦ㄨ繖涓鏋朵腑锛屾父鎴忓唴瀹逛富瑕佸垎涓哄洓绫诲厓绱狅細鍗曚綅锛圼Units](unit.h)锛夈€侀殰纰嶇墿锛圼Obstacles](obstacle.h)锛夈€佸瓙寮癸紙[Bullets](bullet.h)锛夊拰绮掑瓙锛圼Particles](particle.h)锛夈€�
杩欏洓绫诲厓绱犵殑鍩虹被缁熶竴缁ф壙鑷璞＄被鍨嬶紙[Object](object.h)锛夛紝浠栦滑閮芥湁涓€瀹氱壒瀹氱殑棰勮鍔熻兘锛岀敤浜庢柟渚夸綘瀹炵幇鑷繁鐨勬兂娉曘€�

## Object

瀵硅薄绫荤殑澹版槑浣嶄簬 [object.h](object.h) 鏂囦欢涓紝
浣犲彲浠ョ湅鍒板叾鍖呭惈浜嗕互涓嬫垚鍛樺彉閲忥細

```c++
class Object {
    ...
protected:
    GameCore *game_core_{nullptr};
    glm::vec2 position_{0.0f};  // offset from the origin (0, 0)
    float rotation_{0.0f};      // angle in radians
    uint32_t id_{0};
};
```

- game_core_
  - 杩欐槸涓€涓寚鍚戞父鎴忔牳蹇冪被鐨勬寚閽堬紝浼氬湪瀵硅薄琚垱寤烘椂璧嬪€笺€傚綋瀵硅薄鍦ㄨ繍琛岃繃绋嬩腑闇€瑕佷笌娓告垙鍐呭叾浠栧厓绱犱氦浜掓椂锛�
浣犲彲浠ラ€氳繃杩欎釜鎸囬拡璋冪敤娓告垙鏍稿績鐨勭浉搴斿姛鑳藉嚱鏁般€�
  - 绾﹀畾锛氳鐢ㄤ簬娓告垙涓殑瀵硅薄锛実ame_core_ 涓€瀹氭寚鍚戝悎娉曠殑瀵硅薄锛屼笖鍦ㄨ繃绋嬩腑涓嶆洿鏀瑰叾鍊笺€�
  - 绾﹀畾锛歡ame_core_ 涓� nullptr 鏃讹紝鍒欏綋鍓嶅璞℃槸鍦ㄨ繘琛屼竴浜涙父鎴忓唴瀹瑰鐨勬祴璇曪紝姝ゆ椂璇ュ璞℃父鎴忛€昏緫閮ㄥ垎浠ｇ爜涓嶄細涔熶笉搴旇琚皟鐢ㄣ€�
- id_
  - 琛ㄧず瀵硅薄鐨勭紪鍙凤紝鍚岀被鍏冪礌鐨勪笉鍚屽疄浣撴嫢鏈変笉鍚岀殑缂栧彿锛屼笉鍚岀被鍨嬪厓绱犵殑瀹炰綋鍙兘鎷ユ湁鐩稿悓缂栧彿銆�
  渚嬪锛屽睘浜庡崟浣� Unit 瀛愮被鐨勫疄浣擄紝鏈彉閲忚〃绀虹殑鍊间负**鍗曚綅缂栧彿**锛圲nit ID锛夛紝浠ユ涓虹被姣旓紝閫傜敤浜庡叾浠栦笁绫诲厓绱犮€�
  - 绾﹀畾锛氫换浣曠被鍨嬪厓绱犵殑缂栧彿 0 涓轰繚鐣欑紪鍙凤紝鐢ㄤ簬涓€浜涚壒娈婂垽鏂殑澶勭悊銆�
- position_, rotation_
  - 娓告垙涓瘡涓€涓璞￠兘鎷ユ湁鍏朵綅缃俊鎭�
  - position_ 琛ㄧず瀵硅薄鐩稿浜庢父鎴忓唴鍧愭爣鍘熺偣鐨勫亸绉婚噺
  - glm::vec2 鏄� glm 鏁板杩愮畻搴撲腑琛ㄧず鍗曠簿搴︽诞鐐逛簩缁村悜閲忕殑绫诲瀷锛岃繖涓暟瀛﹀簱鍐呭寘鍚簡鍥涚淮鍙婁互涓嬪悜閲忋€佺煩闃电殑绫诲瀷瀹氫箟鍜屽父鐢ㄥ姛鑳藉嚱鏁般€�
  - rotation_ 琛ㄧず瀵硅薄鍦ㄥ満鏅唴鐩告瘮浜庨粯璁ゆ湞鍚戯紝閫嗘椂閽堟棆杞殑瑙掑害

闄ゆ垚鍛樺彉閲忓锛屾墍鏈夊璞￠兘搴斿寘鍚袱閮ㄥ垎鍔熻兘锛�

```c++
class Object {
public:
    ...
    virtual void Update() = 0;
    virtual void Render() = 0;
};
```


- Update
  - 杩欎釜鍑芥暟鐢ㄤ簬鏇存柊瀵硅薄鐨勭姸鎬�
  - 瀵硅薄鐨勪富瑕侀€昏緫浠ｇ爜閮藉簲鍐欏湪杩欓噷
  - 杩欎釜鍑芥暟浼氬湪姣忎竴涓父鎴忓抚锛圱ick锛夋洿鏂版椂琚皟鐢�
- Render
  - 杩欎釜鍑芥暟鐢ㄤ簬缁樺埗瀵硅薄鐨勮瑙夋晥鏋�
  - 浣犲彲浠ヨ皟鐢� `src/battle_game/graphics/grahics.h` 涓殑鍑芥暟杩涜鐢婚潰鐨勭粯鍒讹紝
  鍓嶅線 [src/battle_game/graphics/README.md](../graphics/README.md) 鏌ョ湅鐩稿叧缁樺浘鍑芥暟鐨勪娇鐢ㄦ柟娉曘€�

涓轰簡鏂逛究鏋勬€濓紝鎴戜滑鍙互鎯宠薄鎵€鏈夊璞￠兘鏍规嵁鍏朵綅缃俊鎭嫢鏈変竴涓嚜宸辩殑**鏈湴绌洪棿**鍧愭爣绯伙紝
鐩稿浜�**涓栫晫绌洪棿**鍧愭爣绯伙紝鍦�**鏈湴绌洪棿**鍧愭爣绯讳笂杩涜璁＄畻浼氬甫鏉ヨ澶氫究鍒┿€�
鍥犳 Object 绫昏繕鎻愪緵浜嗗涓嬩袱涓嚱鏁扮敤浜庤緟鍔╄绠�

- LocalToWorld
  - 浼犲叆鍙傛暟涓�**鏈湴绌洪棿**鍧愭爣
  - 杩斿洖瀵瑰簲鐨�**涓栫晫绌洪棿**鍧愭爣
- WorldToLocal
  - 浼犲叆鍙傛暟涓�**涓栫晫绌洪棿**鍧愭爣
  - 杩斿洖瀵瑰簲鐨�**鏈湴绌洪棿**鍧愭爣

## Unit

鍗曚綅绫诲０鏄庡湪 [unit.h](unit.h) 涓紝鍗曚綅琛ㄧず娓告垙涓繘琛屼富鍔ㄥ姩浣滅殑瀵硅薄锛屾墍鏈夊叿浣撳崟浣嶇殑瀹炵幇閮藉簲缁ф壙鑷鍩虹被銆�

### 鎴愬憳鍙橀噺

- player_id_
  - 杩欎釜鍙橀噺琛ㄧず鍗曚綅鎵€鏈夎€呯殑**鐜╁缂栧彿**锛圥layer ID锛夛紝鐢ㄤ簬鍖哄垎鏁屾垜
- health_
  - 杩欎釜鍙橀噺琛ㄧず鍗曚綅鐨勭敓鍛藉€�
  - 鍙栧€艰寖鍥翠负 [0, 1]锛屽嵆鍓╀綑鐢熷懡鐩稿浜庢渶澶х敓鍛藉€肩敓鍛藉€肩殑姣斾緥銆�
  - 鍗曚綅瀹為檯鐢熷懡鍊间负 `GetMaxHealth() * health_`
  - 杩欐牱瀹氫箟鏄负浜嗘柟渚垮姩鎬佸湴瀵规渶澶у０鏄庡€艰繘琛岃皟鏁达紝浠ュ疄鐜颁竴浜涘鏉傛満鍒躲€�
  - 璇ュ€煎綊 0 鏃跺崟浣嶆浜°€�
- lifebar_*
  - 杩欎簺鍙橀噺淇濆瓨浜嗙敓鍛芥潯鐨勮缃紝璇烽€氳繃set鏉ヤ慨鏀�
- fadeout_health_
  - 淇濆瓨鐢熷懡鏉℃笎鍙樼殑璧峰浣嶇疆銆傦紙涓€鑸笉闇€瑕佷慨鏀癸級

### 鎴愬憳鍑芥暟

- SetPosition
  - 璁剧疆鍗曚綅浣嶇疆鐨勫嚱鏁�
  - 涓轰簡娓告垙甯ф洿鏂版椂鐨勬暟鎹竴鑷存€э紝涓€鑸笉鐩存帴璋冪敤锛岃€屾槸鐢� GameCore 涓殑 PushEventMoveUnit 鍑芥暟娣诲姞鍗曚綅绉诲姩浜嬩欢
- SetRotation
  - 璁剧疆鍗曚綅鏈濆悜鐨勫嚱鏁�
  - 涓轰簡娓告垙甯ф洿鏂版椂鐨勬暟鎹竴鑷存€э紝涓€鑸笉鐩存帴璋冪敤锛岃€屾槸鐢� GameCore 涓殑 PushEventRotateUnit 鍑芥暟娣诲姞鍗曚綅鏃嬭浆浜嬩欢
- GetDamageScale
  - 鑾峰彇鍗曚綅鐨勪激瀹冲€嶇巼锛岄粯璁や负 1.0
  - 浣犲彲浠ョ紪鍐欒繖閮ㄥ垎鐨勮绠楅€昏緫浠ュ疄鐜伴珮鍊嶇巼鏀诲嚮鍔涘睘鎬с€佸厜鐜瓑鏁堟灉
  - 鍗曚綅閫犳垚浼ゅ鐨勫姛鑳介兘搴旇€冭檻姝ゅ嚱鏁扮殑褰卞搷
- GetSpeedScale
  - 鑾峰彇鍗曚綅鐨勭Щ鍔ㄩ€熷害鍊嶇巼锛岄粯璁や负 1.0
  - 浣犲彲浠ョ紪鍐欒繖閮ㄥ垎鐨勮绠楅€昏緫浠ュ疄鐜板姞閫熴€佸噺閫熺浉鍏崇殑灞炴€с€佸厜鐜姛鑳�
- BasicMaxHealth
  - 杩欐槸涓€涓櫄鍑芥暟
  - 鏈€澶х敓鍛藉€煎熀鍑嗭紝榛樿涓� 100.0
  - 琛ㄧず鐢熷懡鍊煎熀纭€鏁板€�
  - 浣犲彲浠ュ湪涓嶅悓鍗曚綅鐨勫疄鐜颁腑閫氳繃 `override` 淇敼鍗曚綅鐨勫熀纭€鐢熷懡鍊�
- GetHealthScale
  - 鐢熷懡鍊煎€嶇巼锛岄粯璁や负 1.0
  - 浣犲彲浠ョ紪鍐欒繖閮ㄥ垎鐨勮绠楅€昏緫浠ュ疄鐜板寮烘垨琛板急鐢熷懡鍊肩殑鍔熻兘
- GetMaxHealth
  - 鏈€澶х敓鍛藉€�
  - 瀹氫箟涓哄熀纭€鐢熷懡鍊间箻浠ョ敓鍛藉€煎€嶇巼
- Set/GetLifeBar*
  - 淇敼/鑾峰彇鍚勭鐢熷懡鏉¤缃�
- RenderLifeBar
  - 杩欐槸涓€涓櫄鍑芥暟
  - 娓叉煋璇ュ璞″搴旂殑鐢熷懡鏉�
- Hide/ShowLifeBar
  - 闅愯棌/鏄剧ず鐢熷懡鏉�
- RenderHelper
  - 杩欐槸涓€涓櫄鍑芥暟
  - 浠呭湪璇ュ崟浣嶆墍鏈夎€呯帺瀹剁殑瑙嗚涓紝娓叉煋璇ュ璞＄敤浜庤緟鍔╃殑涓€浜涜瑙夋晥鏋滐紙渚嬪瀛愬脊灏勫嚭鐨勯璁¤建杩癸級
- IsHit
  - 杩欐槸涓€涓櫄鍑芥暟
  - 鐢ㄤ簬鍒ゆ柇鑻ヤ竴涓簨浠跺彂鐢熶簬浼犲叆鍙傛暟**涓栫晫绌洪棿**鍧愭爣 `position`锛岃浜嬩欢鏄惁浼氬褰撳墠鍗曚綅浜х敓褰卞搷
  - 鍙互鐞嗚В涓哄畾涔夊懡涓綋绉殑鍑芥暟
  - 閫氬父鐢ㄤ簬鍒ゆ柇瀛愬脊銆侀灏勭墿鏄惁鍛戒腑浜嗗綋鍓嶅崟浣�
  - 浣犲彲浠ュ湪瀛愮被瀹炵幇涓€氳繃 `override` 瀹氫箟涓嶅悓鐨勫懡涓垽鏂€昏緫
- GenerateBullet
  - 杩欐槸涓€涓ā鏉垮嚱鏁�
  - 鐢变簬瀛愬脊绫荤殑鐢熸垚鍖呭惈浜嗚澶氱被浼间簬鍙戝嚭鑰�**鍗曚綅缂栧彿**銆佹墍灞�**鐜╁缂栧彿**鐩稿叧鐨勫浐瀹氫俊鎭紝鎴戜滑甯屾湜鎶婂熀鏈俊鎭浉鍏崇殑鍐呭鐪佺暐锛屼粠鑰屽彧閫氳繃浼犲叆鍏抽敭淇℃伅鍗冲彲鐢熸垚涓€棰楀瓙寮癸紝
  璇ュ嚱鏁板彲浠ュ府鍔╀綘閫氳繃鍙～鍐欐柊鐢熸垚鐨勫瓙寮瑰璞＄殑浣嶇疆銆佹湞鍚戙€佷激瀹冲€嶇巼绛夊叧閿弬鏁帮紝鑷姩鏍规嵁鍗曚綅淇℃伅杩涜琛ュ叏骞舵坊鍔犱竴涓瓙寮圭敓鎴愪簨浠躲€�
  - 瀹炵幇绫讳技浜庘€滃紑鐏€濅竴绫荤殑鎶€鑳藉彲浠ヤ娇鐢ㄨ鍑芥暟
  - 鍑芥暟鐨勫疄鐜颁綅浜� `src/battle_game/core/game_core.h` 涓�
- Skill
  - units鏀寔鍔犲叆鎶€鑳姐€�
  - 涓轰簡鏂逛究鐜╁鎿嶄綔锛屾妧鑳藉簲褰撲娇鐢ㄩ敭鐩樺揩鎹烽敭瀹屾垚銆傜壒鍒湴锛岀敱浜庢湰娓告垙浣跨敤W/A/S/D鎺у埗杞悜锛屼负鏂逛究璧疯锛屾妧鑳介噰鐢ㄦ寜閿瓻/Q/R瀹屾垚銆傛垜浠瀹欵/Q/R琛ㄧず鐨勬妧鑳藉己搴﹂€掑锛屽苟寤鸿鎸夌収E/Q/R鐨勯『搴忎緷娆″疄鐜版妧鑳斤紙鍙笉瓒�3涓紝浣嗕富鍔ㄦ妧鑳戒竴鑸笉浼氳秴杩�3涓級銆傛澶栵紝P琛ㄧず琚姩鎶€鑳斤紝杩欎竴鎶€鑳戒笉闇€瑕佺敤鎴疯緭鍏ャ€�
  - 鐢ㄦ埛閫氬父甯屾湜浠嶶I鐣岄潰鑾峰彇鎶€鑳界殑绠€鐣ヤ俊鎭€傚洜姝わ紝濡傛灉鎮ㄤ笉甯屾湜鎶€鑳借灞曠ず鍦║I鐣岄潰涓紝璇蜂娇鐢ˋDD_SELECTABLE_UNIT_WITHOUT_SKILL()杩涜璋冪敤锛堝鏋滄偍娌℃湁璁剧疆鎶€鑳斤紝鍒欎笉浼氭樉绀轰换浣曚俊鎭€傚洜姝ゆ妧鑳界晫闈㈠悜鍓嶅吋瀹癸級銆傛澶栵紝鎮ㄩ渶瑕佺淮鎶や竴涓悕绉颁负skill_鐨勪俊鎭瓨鍌ㄥ簱锛屽畠宸茬粡鏄偍鐨剈nits绫诲瀷涓殑protected绫诲瀷銆傚畠鐨勬牸寮忎负std::vector<battle_game::Skill> 銆傚叾涓璖kill鏄竴涓敤浜庝氦浜掔殑缁撴瀯浣撱€�
  - units鏀寔瀛愬脊鍒囨崲鐣岄潰鏄剧ず銆備竴涓猽nits鍙互鎷ユ湁涓嶆涓€绉嶅皠鍑荤殑瀛愬脊锛屽苟涓斿瓙寮瑰垏鎹㈤€氬父闇€瑕佷竴瀹氱殑鍐峰嵈鏃堕棿銆傚鏋滄偍涓嶅笇鏈涘睍绀哄瓙寮圭晫闈紝鎮ㄥ彧闇€瑕佺暀绌哄嵆鍙€� 濡傛灉鎮ㄥ笇鏈涘睍绀哄瓙寮圭晫闈紝璇峰湪skill_閲屽姞鍏ype=B鐨勪竴涓厓绱犮€傚鏋滄湁涓嶆涓€涓瓙寮癸紝璇蜂笉瑕佸娆℃坊鍔狅紝鑰屾槸濉啓褰撳墠鐨刡ullet_type鍜屼竴鍏辩殑bullet_total_number銆傚瓙寮圭晫闈㈠鎮ㄧ殑杈撳叆鍏锋湁涓€瀹氱殑閫傚簲鎬э紝渚嬪锛屽鏋滄偍鍙湁涓€绉嶅瓙寮癸紝灏嗕笉浼氬睍绀哄垏鎹俊鎭紱濡傛灉鎮ㄧ殑鍐峰嵈鏃堕棿涓�0锛屽皢涓嶄細灞曠ず鍐峰嵈杩涘害銆�
``` cpp
enum SkillType { E, Q, R, P, B };
struct Skill {
  std::string name;
  std::string description;
  std::string src;
  uint32_t time_remain;
  uint32_t time_total;
  uint32_t bullet_type;
  uint32_t bullet_total_number;
  SkillType type;
  std::function<void(void)> function;
};
```

 -
    - 浣犻渶瑕佸湪name涓～鍐欐妧鑳藉悕绉帮紝description涓烘妧鑳界畝杩帮紙鑻ユ湁锛夛紝src涓烘妧鑳藉浘绀鸿矾寰勶紙鑻ユ湁锛夛紝time_remain涓烘妧鑳藉喎鍗存椂闂达紝time_total涓烘妧鑳藉喎鍗存€绘椂闂达紝type涓烘妧鑳界被鍨嬶紝function涓烘妧鑳借皟鐢ㄧ殑鎺ュ彛锛堝彲閫夋嫨涓嶆彁渚涳級銆傝嫢閫夋嫨鎻愪緵锛屼娇鐢ㄦ牸寮忎负example.function=SKILL_ADD_FUNCTION(YourUnits::YourFunction)銆�
    - 浣跨敤绀轰緥璇峰弬鑰僫nferno_tank绫诲瀷銆傛妧鑳芥樉绀洪〉闈㈠彲鑳戒細鎸佺画鏇存柊锛屼絾鍙互鎵胯skill_杩欎竴浜や簰瀹瑰櫒浼氫繚鎸佷笉鍙樸€備篃鍗虫妧鑳芥樉绀洪〉闈㈢殑鏇存柊浼氳嚜鍔ㄥ吋瀹规偍鐨勬暟鎹紝鎮ㄦ棤椤诲啀娆＄紪鍐欍€傚鏋滄偍鍙戠幇浜嗘樉绀洪〉闈㈢殑BUG鎴栬€呭笇鏈涘鍔犳洿澶氬唴瀹癸紙濡傛偍鍙兘甯屾湜鍔犲叆鐢ㄦ埛鐘舵€侊紝濡傚姞閫�/鐏肩儳绛夛級锛屾杩庤仈绯籜uGW-Kevin銆�
## Obstacle

闅滅鐗╃被澹版槑鍦� [obstacle.h](obstacle.h) 涓紝璇ョ被瀵硅薄涓昏鐢ㄤ簬缁勬垚娓告垙鍦烘櫙銆�

- GetSurfaceNormal
  - 缁欏畾鍚戦噺鐨勫鐐逛笌缁堢偣淇℃伅锛岃繑鍥炲悜閲忎笌鐗╀綋琛ㄩ潰鐩镐氦澶勫搴旂殑琛ㄩ潰鍗曚綅娉曞悜閲忎俊鎭€傛垜浠害瀹氬叾杩斿洖鍊肩殑绗竴椤逛负缁欏畾绾挎涓庤〃闈㈢殑浜ょ偣锛岀浜岄」涓哄崟浣嶆硶鍚戦噺鐨勬柟鍚戙€�
  - 涓昏鐢ㄤ簬瀹炵幇瀛愬脊鐨勫弽寮广€傚浜庨殰纰嶇墿锛堝浜庢煇浜涘瓙寮癸級涓嶅簲璇ヨ繑鍥炲搴旇〃闈㈠崟浣嶆硶鍚戦噺鐨勬儏鍐碉紝鎴戜滑绾﹀畾杩斿洖鍊间腑鍗曚綅娉曞悜閲忕殑鏂瑰悜璁句负 (0,0)銆�

### 鎴愬憳鍑芥暟

- IsBlocked
  - 杩欐槸涓€涓櫄鍑芥暟
  - 璇ュ嚱鏁扮敤浜庡垽鏂紶鍏ュ弬鏁�**涓栫晫绌洪棿**鍧愭爣 `position` 鏄惁琚闅滅鐗╅樆鎸�
  - 浣犲彲浠ュ湪瀛愮被瀹炵幇涓€氳繃 `override` 瀹氫箟涓嶅悓鐨勯殰纰嶇墿浣滅敤妯″紡

## Bullet

瀛愬脊绫诲０鏄庡湪 [bullet.h](bullet.h) 鏂囦欢涓紝璇ョ被瀵硅薄涓昏鐢ㄤ簬琛ㄧず浼氬娓告垙鎬т骇鐢熷奖鍝嶇殑涓存椂瀵硅薄銆傚锛氬瓙寮�

鐩告瘮浜庡瓙寮硅繖涓悕绉帮紝鎴栬鐢ㄢ€滈灏勭墿鈥濅竴璇嶆洿绗﹀悎鍏跺箍涔夌殑鍚箟銆�

### 鎴愬憳鍙橀噺

- unit_id_
  - 琛ㄧず璇ラ灏勭墿鐨勫垱寤鸿€呯殑**鍗曚綅缂栧彿**锛圲nit ID锛夛紝鐢ㄤ簬鍙兘瀹炵幇鐨勨€滅粡楠屽€肩郴缁熲€濇垨鐩稿叧鏈哄埗
- player_id_
  - 琛ㄧず璇ラ灏勭墿鐨勬墍灞炵帺瀹剁殑**鐜╁缂栧彿**锛圥layer ID锛夛紝鐢ㄤ簬鍖哄垎闃佃惀
- damage_scale_
  - 浼ゅ鍊嶇巼锛岀敤浜庡瓨鍌ㄨ瀛愬脊琚垱寤烘椂琚祴浜堢殑浼ゅ鍊嶇巼锛屼互浣滅敤浜庡懡涓椂鐨勫疄闄呬激瀹�


## Particle

绮掑瓙绫诲０鏄庡湪 [particle.h](particle.h) 鏂囦欢涓紝璇ョ被瀵硅薄涓昏鐢ㄤ簬琛ㄧず涓嶄細瀵规父鎴忔€т骇鐢熷奖鍝嶇殑涓存椂瀵硅薄锛岀敤浜庢彁鍗囪瑙変綋楠屻€�

## GameCore

娓告垙鏍稿績鎻愪緵浜嗚澶氱敤浜庢柟渚挎父鎴忓唴鍏冪礌杩涜浜や簰鐨勫嚱鏁板姛鑳�

### 瀵硅薄璁块棶

閫氳繃娓告垙鏍稿績瀵硅薄浣犲彲浠ヨ闂叾浠栦换鎰忎粛鏃у瓨鍦ㄤ簬娓告垙涓殑瀵硅薄锛�

- GetUnit
  - 鏍规嵁杈撳叆鐨�**鍗曚綅缂栧彿**锛圲nit ID锛夛紝杩斿洖瀵瑰簲鍗曚綅瀵硅薄鐨勬寚閽�
  - 鑻�**鍗曚綅缂栧彿**鎸囧悜鐨勫崟浣嶅凡缁忚鍒犻櫎鎴栫紪鍙蜂笉鍚堟硶锛屽垯杩斿洖 `nullptr`
- GetUnits
  - 鑾峰彇鎵€鏈夌幇瀛樼殑鍗曚綅
- GetObstacle
    - 鏍规嵁杈撳叆鐨�**闅滅鐗╃紪鍙�**锛圤bstacle ID锛夛紝杩斿洖瀵瑰簲闅滅鐗╁璞＄殑鎸囬拡
    - 鑻�**闅滅鐗╃紪鍙�**鎸囧悜鐨勯殰纰嶇墿宸茬粡琚垹闄ゆ垨缂栧彿涓嶅悎娉曪紝鍒欒繑鍥� `nullptr`
- GetObstacles
  - 鑾峰彇鎵€鏈夌幇瀛樼殑闅滅鐗�
- GetBlockedObstacle
  - 鍒ゆ柇浼犲叆鍙傛暟**涓栫晫绌洪棿**鍧愭爣 `position` 鏄惁琚煇闅滅鐗╅樆鎸°€傚鏋滄槸锛岃繑鍥炲搴旈殰纰嶇墿瀵硅薄鐨勬寚閽�
- GetBullet
    - 鏍规嵁杈撳叆鐨�**瀛愬脊缂栧彿**锛圔ullet ID锛夛紝杩斿洖瀵瑰簲瀛愬脊瀵硅薄鐨勬寚閽�
    - 鑻�**瀛愬脊缂栧彿**鎸囧悜鐨勫瓙寮瑰凡缁忚鍒犻櫎鎴栫紪鍙蜂笉鍚堟硶锛屽垯杩斿洖 `nullptr`
- GetBullets
  - 鑾峰彇鎵€鏈夌幇瀛樼殑瀛愬脊
- GetParticle
    - 鏍规嵁杈撳叆鐨�**绮掑瓙缂栧彿**锛圥article ID锛夛紝杩斿洖瀵瑰簲绮掑瓙瀵硅薄鐨勬寚閽�
    - 鑻�**绮掑瓙缂栧彿**鎸囧悜鐨勭矑瀛愬凡缁忚鍒犻櫎鎴栫紪鍙蜂笉鍚堟硶锛屽垯杩斿洖 `nullptr`
- GetParticles
  - 鑾峰彇鎵€鏈夌幇瀛樼殑绮掑瓙

### 浜嬩欢闃熷垪

鐢变簬涓€涓父鎴忓抚鍦ㄦ洿鏂扮殑杩囩▼涓紝涓嶅悓瀵硅薄鐨勬洿鏂板嚱鏁板瓨鍦ㄨ璋冪敤鐨勫厛鍚庨『搴忋€�
涓轰簡鏁版嵁涓€鑷存€э紝鎴戜滑涓嶅笇鏈涘厛琚皟鐢ㄧ殑鏇存柊鍑芥暟浜х敓鐨勫奖鍝嶇珛鍒诲奖鍝嶅埌鍚庣画琚皟鐢ㄧ殑鏇存柊鍑芥暟锛�
渚嬪锛氬綋涓や釜鍗曚綅鍙互鍚屾椂鍑绘潃瀵规柟鏃讹紝鍏堣璋冪敤鐨勫璞″鏋滅珛鍗冲瀵规柟浜х敓浜嗕激瀹筹紝閭ｄ箞鍚庡彂鍗曚綅鐩存帴姝讳骸锛屼細褰卞搷鍒版父鎴忓叕骞炽€�
鍥犳锛屾垜浠笇鏈涙父鎴忓璞＄殑鏇存柊閫昏緫閮ㄥ垎灏嗗彲鑳藉奖鍝嶅埌鍏朵粬瀵硅薄鐨勪簨浠跺欢鍚庣粨绠楋紝鍥犳鎴戜滑寮曞叆浜嗕簨浠堕槦鍒楁満鍒讹細

鍦ㄦ墍鏈夋父鎴忓璞＄殑鏇存柊鍑芥暟璋冪敤瀹屾垚鍚庯紝浜嬩欢闃熷垪浼氫緷娆℃墽琛屽叾涓殑鍐呭銆�
鎴戜滑甯屾湜淇濊瘉锛氫簨浠朵笌浜嬩欢涔嬮棿鐨勭浉瀵归『搴忓璁＄畻缁撴灉娌℃湁骞叉壈銆�

鎬昏€岃█涔嬶紝杩欐槸涓€涓繚璇佹父鎴忛€昏緫涓€鑷存€х殑鏈哄埗锛屽凡缁忔彁渚涚殑浜嬩欢鍑芥暟濡備笅锛�

```c++
void PushEventMoveUnit(uint32_t unit_id, glm::vec2 new_position);
void PushEventRotateUnit(uint32_t unit_id, float new_rotation);
void PushEventDealDamage(uint32_t dst_unit_id,
                           uint32_t src_unit_id,
                           float damage);
void PushEventKillUnit(uint32_t dst_unit_id, uint32_t src_unit_id);
void PushEventRemoveObstacle(uint32_t obstacle_id);
void PushEventRemoveBullet(uint32_t bullet_id);
void PushEventRemoveParticle(uint32_t particle_id);
void PushEventRemoveUnit(uint32_t unit_id);

template <class BulletType, class... Args>
void PushEventGenerateBullet(uint32_t unit_id,
                             uint32_t player_id,
                             glm::vec2 position,
                             float rotation = 0.0f,
                             float damage_scale = 1.0f,
                             Args... args);
```

- PushEventMoveUnit
  - 鍘嬪叆涓€涓崟浣嶇Щ鍔ㄤ簨浠�
- PushEventRotateUnit
  - 鍘嬪叆涓€涓崟浣嶆棆杞簨浠�
- PushEventDealDamage
  - 鍘嬪叆涓€涓激瀹充簨浠�
- PushEventKillUnit
  - 鍘嬪叆涓€涓嚮鏉€浜嬩欢
  - 杩欑浜嬩欢閫氬父閫氳繃浼ゅ浜嬩欢浜х敓
- PushEventRemoveObstacle
  - 鍘嬪叆涓€涓殰纰嶇墿绉婚櫎浜嬩欢
- PushEventRemoveBullet
  - 鍘嬪叆涓€涓瓙寮圭Щ闄や簨浠�
- PushEventRemoveParticle
  - 鍘嬪叆涓€涓矑瀛愮Щ闄や簨浠�
- PushEventRemoveUnit
  - 鍘嬪叆涓€涓崟浣嶇Щ闄や簨浠�
  - 杩欑浜嬩欢閫氬父閫氳繃鍑绘潃浜嬩欢浜х敓
- PushEventGenerateBullet
  - 鍘嬪叆涓€涓瓙寮圭敓鎴愪簨浠�
  - 鍜� Unit 绫讳腑鐨� GenerateBullet 鍑芥暟鐩歌仈绯�

### 闅忔満鍙橀噺

娓告垙涓垜浠父甯镐細瑙佸埌闅忔満鏈哄埗锛屼负浜嗘柟渚挎棩鍚庢垜浠姞鍏ヨ仈鏈哄姛鑳斤紝鎴戜滑闇€瑕佷娇闅忔満鏈哄埗鍏锋湁涓€鑷存€с€�
鍥犳鎴戜滑甯屾湜浣犵紪鍐欑殑浠ｇ爜涓墍鏈夋秹鍙婂埌鐨勯殢鏈哄彉閲忛兘閫氳繃浠ヤ笅鍑芥暟鑾峰緱锛�

- RandomFloat
  - 浜х敓涓€涓尯闂� [0, 1] 鍐呭潎鍖€鍒嗗竷鐨勯殢鏈烘诞鐐规暟
- RandomInt
  - 浜х敓涓€涓尯闂� [low_bound, high_bound] 鍐呭潎鍖€鍒嗗竷鐨勯殢鏈烘暣鏁�
- RandomOnCircle
  - 浜х敓涓€涓崟浣嶅渾鍦嗗懆涓婂潎鍖€鍒嗗竷鐨勯殢鏈哄悜閲�
- RandomInCircle
  - 浜х敓涓€涓崟浣嶅渾鍐呴儴鍧囧寑鍒嗗竷鐨勯殢鏈哄悜閲�
