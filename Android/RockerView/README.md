@[TOC](Android摇杆控件)

# 功能
![在这里插入图片描述](https://img-blog.csdnimg.cn/20210527144852587.gif)
xml:
```xml
<?xml version="1.0" encoding="utf-8"?>
<LinearLayout xmlns:android="http://schemas.android.com/apk/res/android"
    xmlns:app="http://schemas.android.com/apk/res-auto"
    xmlns:tools="http://schemas.android.com/tools"
    android:layout_width="match_parent"
    android:layout_height="match_parent"
    android:orientation="vertical"
    tools:context=".ControlEp">
    <TextView
        android:id="@+id/msg"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"/>
    <Button
        android:id="@+id/show"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"/>
    <com.leotoe.rockerview.MyRockerView
        android:id="@+id/rocker_view"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        app:rockerSpeedLevel="10"
        app:rockerCallBackMode="CALL_BACK_MODE_STATE_CHANGE"
        app:rockerScale="0.5"/>

    <Button
        android:id="@+id/startReceiver"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"/>
    <Button
        android:id="@+id/stopReceiver"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"/>
</LinearLayout>
```
java:
```java
class MainActivity : AppCompatActivity() {
    companion object {
        const val TAG = "MainActivity"
    }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val mRockerViewXY = findViewById<MyRockerView>(R.id.rockerXY_View)  // 1
        val mRockerViewZ = findViewById<MyRockerView>(R.id.rockerZ_View)    // 2
        val directionXY_Text = findViewById<TextView>(R.id.directionXY_Text)    // 1当前方向
        val angleXY_Text = findViewById<TextView>(R.id.angleXY_Text)    // 1当前角度
        val levelXY_Text = findViewById<TextView>(R.id.levelXY_Text)    // 1当前偏移级别
        val directionZ_Text = findViewById<TextView>(R.id.directionZ_Text) // 2当前方向
        val angleZ_Text = findViewById<TextView>(R.id.angleZ_Text) // 2当前角度
        val levelZ_Text = findViewById<TextView>(R.id.levelZ_Text) // 2当前偏移级别
        
        // 1
        mRockerViewXY.setOnShakeListener(MyRockerView.DirectionMode.DIRECTION_8, object : MyRockerView.OnShakeListener {
            override fun onStart() {}
            override fun direction(direction: MyRockerView.Direction) {
                var directionXY = ""
                when (direction) {
                    MyRockerView.Direction.DIRECTION_CENTER -> {
                        directionXY = "当前方向：中心"
                        angleXY_Text.text = "当前方向："
                        levelXY_Text.text = "当前偏移级别："
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
                    MyRockerView.Direction.DIRECTION_DOWN_LEFT -> {
                        directionXY = "当前方向：左下"
                    }
                    MyRockerView.Direction.DIRECTION_DOWN_RIGHT -> {
                        directionXY = "当前方向：右下"
                    }
                    MyRockerView.Direction.DIRECTION_UP_LEFT -> {
                        directionXY = "当前方向：左上"
                    }
                    MyRockerView.Direction.DIRECTION_UP_RIGHT -> {
                        directionXY = "当前方向：右上"
                    }
                }
                Log.e(TAG, "XY轴$directionXY")
                Log.e(TAG, "-----------------------------------------------")
                directionXY_Text.text = directionXY
            }
            override fun onFinish() {}
        })
        // 1
        mRockerViewXY.setOnAngleChangeListener(object : MyRockerView.OnAngleChangeListener {
            override fun onStart() {}
            override fun angle(angle: Double) {
                var angleXY = ""
                angleXY = "当前角度：$angle"
                Log.e(TAG, "XY轴$angleXY")
                angleXY_Text.setText(angleXY)
            }

            override fun onFinish() {}
        })
        // 1
        mRockerViewXY.setOnDistanceLevelListener { level ->
            var levelXY = ""
            levelXY = "当前距离级别：$level"
            Log.e(TAG, "XY轴$levelXY")
            levelXY_Text.setText(levelXY)
        }


        // 2
        mRockerViewZ.setOnShakeListener(MyRockerView.DirectionMode.DIRECTION_4_ROTATE_45, object : MyRockerView.OnShakeListener {
            override fun onStart() {}
            override fun direction(direction: MyRockerView.Direction) {
                var directionXY = ""
                when (direction) {
                    MyRockerView.Direction.DIRECTION_CENTER -> {
                        directionXY = "当前方向：中心"
                        angleZ_Text.text = "当前方向："
                        levelZ_Text.text = "当前偏移级别："
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

        // 2
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
        // 2
        mRockerViewZ.setOnDistanceLevelListener { level ->
            var levelXY = ""
            levelXY = "当前距离级别：$level"
            Log.e(TAG, "XY轴$levelXY")
            levelZ_Text.setText(levelXY)
        }
    }
}
```

## 实现的监听功能
1.摇杆方向监听
2.摇杆摇动角度监听
3.摇杆摇动距离监听

## 方向模式
实现了摇杆的五中方向模式
DIRECTION_2_HORIZONTAL,// 横向 左右两个方向
DIRECTION_2_VERTICAL, // 纵向 上下两个方向
DIRECTION_4_ROTATE_0, // 四个方向
DIRECTION_4_ROTATE_45, // 四个方向 旋转45度
DIRECTION_8 // 八个方向

---
***
___
# 用法
## 准备工作
1.在project的build.gradle中添加

```java
	allprojects {
		repositories {
			...
			maven { url 'https://jitpack.io' }
		}
	}
```
2.在module的build.gradle中添加

```java
	dependencies {
	        implementation 'com.gitee.y141111:RockerView:1.0.0'
	}
```
3.xml中添加

```xml
    <com.leotoe.rockerview.MyRockerView
        android:id="@+id/rocker_view"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        app:areaBackground="@color/white"
        app:rockerBackground="@color/black"
        app:rockerSpeedLevel="10"
        app:rockerCallBackMode="CALL_BACK_MODE_STATE_CHANGE"
        app:rockerScale="0.5"/>
```
>areaBackground 设置区域背景
>rockerBackground 设置摇杆样式
>rockerScale 设置摇杆的相对于背景的比例
>rockerSpeedLevel 设置当前位置相对于中心点的距离的比例
>rockerCallBackMode 有变化就回调，或者是方向改变才会回调

4.在Activity中添加
### 八方向示例
#### 1).添加方向监听（方向发生改变触发）
```java
        MyRockerView rockerView = binding.rockerView;
        
        rockerView.setOnShakeListener(MyRockerView.DirectionMode.DIRECTION_8, new MyRockerView.OnShakeListener() {
            @Override
            public void onStart() {
                
            }

            @Override
            public void direction(MyRockerView.Direction direction) {
                switch (direction){
                    case DIRECTION_CENTER:
                        // 中心方向触发事件，一般归零
                        break;
                    case DIRECTION_UP:
                        // 上 触发事件
                        break;
                    case DIRECTION_UP_RIGHT:
                        // 上右 触发事件
                        break;
                    case DIRECTION_RIGHT:
                        break;
                    case DIRECTION_DOWN_RIGHT:
                        break;
                    case DIRECTION_DOWN:
                        break;
                    case DIRECTION_DOWN_LEFT:
                        break;
                    case DIRECTION_LEFT:
                        break;
                    case DIRECTION_UP_LEFT:
                        break;
                }
            }

            @Override
            public void onFinish() {

            }
        });
```
#### 2).添加角度监听（角度发生改变触发）

```java
        rockerView.setOnAngleChangeListener(new MyRockerView.OnAngleChangeListener() {
            @Override
            public void onStart() {
                
            }

            @Override
            public void angle(double angle) {
                // 角度发生改变 触发事件 当前角度 angle
            }

            @Override
            public void onFinish() {

            }
        });
```
#### 3).添加距离监听（距离发生改变触发）

```java
        rockerView.setOnDistanceLevelListener(new MyRockerView.OnDistanceLevelListener() {
            @Override
            public void onDistanceLevel(int level) {
                // 距中心点距离发生改变 触发事件
            }
        });
```

### 四方向示例
>旋转45度才是正常情况下使用的四方向
>
#### 1).添加方向监听（方向发生改变触发）

```java

        MyRockerView rockerView = binding.rockerView;

        rockerView.setOnShakeListener(MyRockerView.DirectionMode.DIRECTION_4_ROTATE_45, new MyRockerView.OnShakeListener() {
            @Override
            public void onStart() {

            }

            @Override
            public void direction(MyRockerView.Direction direction) {
                switch (direction){
                    case DIRECTION_CENTER:
                        // 中心方向触发事件，一般归零
                        break;
                    case DIRECTION_UP:
                        // 上 触发事件
                        break;
                    case DIRECTION_RIGHT:
                        // 右 触发事件
                        break;
                    case DIRECTION_DOWN:
                        // 下 触发事件
                        break;
                    case DIRECTION_LEFT:
                        // 左 触发事件
                        break;
                }
            }

            @Override
            public void onFinish() {

            }
        });
```

#### 2).添加角度监听，角度发生改变触发

```java
        rockerView.setOnAngleChangeListener(new MyRockerView.OnAngleChangeListener() {
            @Override
            public void onStart() {
                
            }

            @Override
            public void angle(double angle) {
                // 角度发生改变 触发事件 当前角度 angle
            }

            @Override
            public void onFinish() {

            }
        });
```
#### 3).添加距离监听（距离发生改变触发）

```java
        rockerView.setOnDistanceLevelListener(new MyRockerView.OnDistanceLevelListener() {
            @Override
            public void onDistanceLevel(int level) {
                // 距中心点距离发生改变 触发事件
            }
        });
```
角度和距离发生改变触发的是相同的监听事件

---
# 源代码
控件已开源，gitee项目地址:[https://gitee.com/y141111/RockerView](https://gitee.com/y141111/RockerView)