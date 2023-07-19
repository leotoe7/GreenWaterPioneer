package com.leotoe.lvshui

import android.annotation.SuppressLint
import android.content.ContentValues.TAG
import android.graphics.Color
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.util.Log
import android.view.View
import android.view.WindowManager
import android.webkit.WebView
import android.webkit.WebViewClient
import android.widget.*
import android.widget.SeekBar.OnSeekBarChangeListener
import androidx.appcompat.app.AppCompatActivity
import com.leotoe.rockerview.MyRockerView
import com.leotoe.rockerview.SeekBarVertical
import okhttp3.*
import org.json.JSONException
import org.json.JSONObject
import java.util.*


class MainActivity : AppCompatActivity() {
//    private lateinit var webSocket: WebSocket
//
//    companion object {
//        private const val TAG = "WebSocketDemo"
//        private const val NORMAL_CLOSURE_STATUS = 1000
//    }

    private fun setFullscreen(isShowStatusBar: Boolean, isShowNavigationBar: Boolean) {
        var uiOptions: Int = (View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                or View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                or View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY)
        if (!isShowStatusBar) {
            uiOptions = uiOptions or View.SYSTEM_UI_FLAG_FULLSCREEN
        }
        if (!isShowNavigationBar) {
            uiOptions = uiOptions or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
        }
        window.decorView.systemUiVisibility = uiOptions

        //隐藏标题栏
        // getSupportActionBar().hide();
        setNavigationStatusColor(Color.TRANSPARENT)
    }

    private fun setNavigationStatusColor(color: Int) {
        //VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP
        window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS)
        window.navigationBarColor = color
        window.statusBarColor = color
    }

    private val timer = Timer()
    private val timer2 = Timer()
    @OptIn(ExperimentalStdlibApi::class)
    var key = Char(0)

    @SuppressLint("UseSwitchCompatOrMaterialCode", "MissingInflatedId")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        //隐藏标题栏
        Objects.requireNonNull(supportActionBar)?.hide()

        setContentView(R.layout.activity_main)
        setFullscreen(isShowStatusBar = false, isShowNavigationBar = true)

        val lng = findViewById<TextView>(R.id.lng)
        val lat = findViewById<TextView>(R.id.lat)
        val alt = findViewById<TextView>(R.id.alt)
        val temperature = findViewById<TextView>(R.id.temperature)
        val ph = findViewById<TextView>(R.id.ph)
        val turbidity = findViewById<TextView>(R.id.turbidity)

        val handler: Handler = @SuppressLint("HandlerLeak")
        object : Handler() {
            @SuppressLint("HandlerLeak")
            override fun handleMessage(msg: Message) {
                when (msg.what) {
                    1 -> {
                        val bundleCode = msg.data
                        temperature.text = bundleCode.getString("gwp_temperature")
                        ph.text = bundleCode.getString("gwp_ph")
                        turbidity.text = bundleCode.getString("gwp_turbidity")
                        lng.text = bundleCode.getString("gwp_lng")
                        lat.text = bundleCode.getString("gwp_lat")
                        alt.text = bundleCode.getString("gwp_alt")
                    }
                    else -> {}
                }
            }
        }



        val timerTask2: TimerTask = object : TimerTask() {
            override fun run() {
                //todo
                object : Thread() {
                    //网络请求需要在子线程中完成
                    @SuppressLint("SetTextI18n")
                    override fun run() {
                        val request = MyRequest()
                        var res = ""
                        //String data = "";//POST请求的参数
                        res = request.get("http://192.168.4.254/gwp")//调用我们写的get方法
                        //Log.d(TAG, res)
                        try {
                            val jsonObject1 = JSONObject(res)
                            val gwp_temperature = jsonObject1.getString("temperature").toFloat()/1000
                            val temperature_text = "温度:$gwp_temperature °C"
                            val gwp_ph = jsonObject1.getString("ph").toFloat()/1000
                            val ph_text = "PH:$gwp_ph"
                            val gwp_turbidity = jsonObject1.getString("turbidity")
                            var turbidity_text = ""
                            if (jsonObject1.getString("turbidity").length > 5) {
                                turbidity_text = "浊度:0 TU"
                            }
                            else{
                                turbidity_text = "浊度:$gwp_turbidity TU"
                            }
                            val satellite = jsonObject1.getString("satellite")
                            val satellite_Object = JSONObject(satellite)
                            val gwp_lng = satellite_Object.getString("lng")
                            val lng_text = "经度:$gwp_lng"
                            val gwp_lat = satellite_Object.getString("lat")
                            val lat_text = "纬度:$gwp_lat"
                            val gwp_alt = satellite_Object.getString("alt").toFloat()/100
                            val alt_text = "海拔:$gwp_alt 米"
                            val msg = Message()
                            val bundle = Bundle()
                            bundle.putString("gwp_temperature", temperature_text)
                            bundle.putString("gwp_ph", ph_text)
                            bundle.putString("gwp_turbidity", turbidity_text)
                            bundle.putString("gwp_lng", lng_text)
                            bundle.putString("gwp_lat", lat_text)
                            bundle.putString("gwp_alt", alt_text)
                            msg.data = bundle
                            msg.what = 1
                            handler.sendMessage(msg)
                            //Log.d("MainActivity", "gwp_lat is $gwp_lat")
                            //Log.d("MainActivity", "version is $version")
                        } catch (_: JSONException) {
                        }
                        Looper.prepare()
                        //Toast.makeText(context!, res, Toast.LENGTH_SHORT).show()
                        Looper.loop()
                    }
                }.start()
            }
        }

        timer.schedule(timerTask, 0, 100) //立刻执行，间隔150ms循环执行
        timer2.schedule(timerTask2, 0, 1500) //立刻执行，间隔1500ms循环执行


        val s = findViewById<View>(R.id.led) as Switch
        s.setOnCheckedChangeListener(CompoundButton.OnCheckedChangeListener { buttonView, isChecked ->
            if (s.isChecked) {
                //Toast.makeText(this@MainActivity, "开启", Toast.LENGTH_LONG).show()
                object : Thread() {
                    //网络请求需要在子线程中完成
                    override fun run() {
                        val request = MyRequest()
                        var res = ""
                        //String data = "";//POST请求的参数
                        res = request.get("http://192.168.4.254/control?var=led_intensity&val=100")//调用我们写的post方法
                        //Log.d(TAG, res)
                        Looper.prepare()
                        //Toast.makeText(context!, res, Toast.LENGTH_SHORT).show()
                        Looper.loop()
                    }
                }.start()
            } else {
                //Toast.makeText(this@MainActivity, "关闭", Toast.LENGTH_LONG).show()
                object : Thread() {
                    //网络请求需要在子线程中完成
                    override fun run() {
                        val request = MyRequest()
                        var res = ""
                        //String data = "";//POST请求的参数
                        res = request.get("http://192.168.4.254/control?var=led_intensity&val=0")//调用我们写的post方法
                        //Log.d(TAG, res)
                        Looper.prepare()
                        //Toast.makeText(context!, res, Toast.LENGTH_SHORT).show()
                        Looper.loop()
                    }
                }.start()
            }
        })

        //LED亮度(0-255) http://10.8.8.7/control?var=led_intensity&val=255
        //彩条(0-1) http://10.8.8.7/control?var=colorbar&val=1
        //质量(4-63) http://10.8.8.7/control?var=quality&val=63
        //亮度(-2-2) http://10.8.8.7/control?var=brightness&val=-1
        //对比度(-2-2) http://10.8.8.7/control?var=contrast&val=0
        //饱和度(-2-2) http://10.8.8.7/control?var=saturation&val=1
        //帧大小(0-13) http://10.8.8.7/control?var=framesize&val=8
        object : Thread() {
            //网络请求需要在子线程中完成
            override fun run() {
                val request = MyRequest()
                var res = ""
                //String data = "";//POST请求的参数
                res = request.get("http://10.8.8.7/control?var=framesize&val=8")//调用我们写的post方法
                //Log.d(TAG, res)
                Looper.prepare()
                //Toast.makeText(context!, res, Toast.LENGTH_SHORT).show()
                Looper.loop()
            }
        }.start()


        //获得控件
        val mw = findViewById<View>(R.id.myweb) as WebView
        // 调用成员函数访问网页，加载资源
        mw.loadUrl("http://192.168.4.254:81/stream")
        val webSettings = mw.settings
        webSettings.displayZoomControls = false //隐藏原生的缩放控件
        //设置自适应屏幕，两者合用
        webSettings.useWideViewPort = true //将图片调整到适合webview的大小
        webSettings.loadWithOverviewMode = true // 缩放至屏幕的大小
        webSettings.builtInZoomControls = true //设置内置的缩放控件。若为false，则该WebView不可缩放
        //系统默认会通过手机浏览器打开网页，为了能够直接通过WebView显示网页，则必须设置
        mw.webViewClient = object : WebViewClient() {
            @Deprecated("Deprecated in Java")
            override fun shouldOverrideUrlLoading(view: WebView, url: String): Boolean {
                // 重写WebViewClient的shouldOverrideUrlLoading()方法
                //使用WebView加载显示url
                view.loadUrl(url)
                //返回true
                return true
            }
        }



        val refresh = findViewById<View>(R.id.refresh) as Button
        refresh.setOnClickListener(View.OnClickListener {
            mw.reload()
        })

        //方向有改变时回调
        val mRockerViewXY = findViewById<MyRockerView>(R.id.rockerXY_View)  // 8方向
//        val mRockerViewZ = findViewById<MyRockerView>(R.id.rockerZ_View)    // 4方向
        val mSeekBarVertical = findViewById<SeekBarVertical>(R.id.seekbarVertical_View)    // seekbar


        val directionXY_Text = findViewById<TextView>(R.id.directionXY_Text)    // 当前方向
        val angleXY_Text = findViewById<TextView>(R.id.angleXY_Text)    // 当前角度
        val levelXY_Text = findViewById<TextView>(R.id.levelXY_Text)    // 当前偏移级别
//        val directionZ_Text = findViewById<TextView>(R.id.directionZ_Text) // 当前方向
//        val angleZ_Text = findViewById<TextView>(R.id.angleZ_Text) // 当前角度
//        val levelZ_Text = findViewById<TextView>(R.id.levelZ_Text) // 当前偏移级别

        // xy轴
        // 1方向
        mRockerViewXY.setOnShakeListener(MyRockerView.DirectionMode.DIRECTION_8, object : MyRockerView.OnShakeListener {
            override fun onStart() {}
            override fun direction(direction: MyRockerView.Direction) {
                var directionXY = ""
                when (direction) {
                    MyRockerView.Direction.DIRECTION_CENTER -> {
                        directionXY = "当前方向：中心"
                        key = 'q'
                    }
                    MyRockerView.Direction.DIRECTION_DOWN -> {
                        directionXY = "当前方向：下"
                        key = 's'
                    }
                    MyRockerView.Direction.DIRECTION_LEFT -> {
                        directionXY = "当前方向：左"
                        key = 'a'
                    }
                    MyRockerView.Direction.DIRECTION_UP -> {
                        directionXY = "当前方向：上"
                        key = 'w'
//                        webSocket.send("w")
                    }
                    MyRockerView.Direction.DIRECTION_RIGHT -> {
                        directionXY = "当前方向：右"
                        key = 'd'
                    }
                    MyRockerView.Direction.DIRECTION_DOWN_LEFT -> {
                        directionXY = "当前方向：左下"
                        key = 'r'
                    }
                    MyRockerView.Direction.DIRECTION_DOWN_RIGHT -> {
                        directionXY = "当前方向：右下"
                        key = 'f'
                    }
                    MyRockerView.Direction.DIRECTION_UP_LEFT -> {
                        directionXY = "当前方向：左上"
                        key = 'a'
                    }
                    MyRockerView.Direction.DIRECTION_UP_RIGHT -> {
                        directionXY = "当前方向：右上"
                        key = 'd'
                    }
                }
                Log.e(TAG, "XY轴$directionXY")
                Log.e(TAG, "-----------------------------------------------")
                directionXY_Text.text = directionXY
            }
            override fun onFinish() {}
        })
        // 1角度
        mRockerViewXY.setOnAngleChangeListener(object : MyRockerView.OnAngleChangeListener {
            override fun onStart() {}
            override fun angle(angle: Double) {
                var angleXY = ""
                angleXY = "当前角度：$angle"
                Log.e(TAG, "XY轴$angleXY")
                angleXY_Text.text = angleXY
            }

            override fun onFinish() {}
        })
        // 1级别
        mRockerViewXY.setOnDistanceLevelListener { level ->
            var levelXY = ""
            levelXY = "当前距离级别：$level"
            Log.e(TAG, "XY轴$levelXY")
            levelXY_Text.text = levelXY
        }

/*
        // 2方向
        mRockerViewZ.setOnShakeListener(MyRockerView.DirectionMode.DIRECTION_4_ROTATE_45, object : MyRockerView.OnShakeListener {
            override fun onStart() {}
            override fun direction(direction: MyRockerView.Direction) {
                var directionXY = ""
                when (direction) {
                    MyRockerView.Direction.DIRECTION_CENTER -> {
                        directionXY = "当前方向：中心"
                    }
                    MyRockerView.Direction.DIRECTION_DOWN -> {
                        directionXY = "当前方向：下"
                    }
                    MyRockerView.Direction.DIRECTION_LEFT -> {
                        directionXY = "当前方向：左"
                    }
                    MyRockerView.Direction.DIRECTION_UP -> {
                        directionXY = "当前方向：上"
                    }
                    MyRockerView.Direction.DIRECTION_RIGHT -> {
                        directionXY = "当前方向：右"
                    }
                    else -> {}
                }
                Log.e(TAG, "XY轴$directionXY")
                Log.e(TAG, "-----------------------------------------------")
                directionZ_Text.text = directionXY
            }
            override fun onFinish() {}
        })

        // 2角度
        mRockerViewZ.setOnAngleChangeListener(object : MyRockerView.OnAngleChangeListener {
            override fun onStart() {}
            override fun angle(angle: Double) {
                var angleXY = ""
                angleXY = "当前角度：$angle"
                Log.e(TAG, "XY轴$angleXY")
                angleZ_Text.setText(angleXY)
            }

            override fun onFinish() {}
        })
        // 2级别
        mRockerViewZ.setOnDistanceLevelListener { level ->
            var levelXY = ""
            levelXY = "当前距离级别：$level"
            Log.e(TAG, "XY轴$levelXY")
            levelZ_Text.setText(levelXY)
        }
 */
        mSeekBarVertical.setOnSeekBarChangeListener(object :OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                println(progress)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
                TODO("Not yet implemented")
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                TODO("Not yet implemented")
            }
        })
    }

    /*
     try {
            JSONArray jsonArray = new JSONArray(jsonData);
            for(int i=0;i<jsonArray.length();i++){
                JSONObject jsonObject = jsonArray.getJSONObject(i);
                String id = jsonObject.getString("id");
                String name = jsonObject.getString("name");
                String version = jsonObject.getString("version");

                Log.d("MainActivity", "id is " + id);
                Log.d("MainActivity", "name is " + name);
                Log.d("MainActivity", "version is " + version);
            }
        } catch (JSONException e) {

        }
    */
    private val timerTask: TimerTask = object : TimerTask() {
        override fun run() {
            //todo
            object : Thread() {
                //网络请求需要在子线程中完成
                override fun run() {
                    val request = MyRequest()
                    var res = ""
                    //String data = "";//POST请求的参数
                    res = request.get("http://192.168.4.1/action?action=$key")//调用我们写的post方法
                    //Log.d(TAG, res)
                    Looper.prepare()
                    //Toast.makeText(context!, res, Toast.LENGTH_SHORT).show()
                    Looper.loop()
                }
            }.start()
        }
    }

}

