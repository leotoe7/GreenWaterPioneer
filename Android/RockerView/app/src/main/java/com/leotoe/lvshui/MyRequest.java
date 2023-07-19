package com.leotoe.lvshui;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

public class MyRequest {

    public String post(String url1, String data) {
        try {
            URL url = new URL(url1);
            HttpURLConnection Connection = (HttpURLConnection) url.openConnection();//创建连接
            Connection.setRequestMethod("POST");
            Connection.setConnectTimeout(3000);
            Connection.setReadTimeout(3000);
            Connection.setDoInput(true);
            Connection.setDoOutput(true);
            Connection.setUseCaches(false);
            Connection.connect();
            DataOutputStream dos = new DataOutputStream(Connection.getOutputStream());
            dos.write(data.getBytes());
            dos.flush();
            dos.close();//写完记得关闭
            int responseCode = Connection.getResponseCode();
            if (responseCode == HttpURLConnection.HTTP_OK) {//判断请求是否成功
                InputStream inputStream = Connection.getInputStream();
                ByteArrayOutputStream arrayOutputStream = new ByteArrayOutputStream();
                byte[] bytes = new byte[1024];
                int length;
                while ((length = inputStream.read(bytes)) != -1) {
                    arrayOutputStream.write(bytes, 0, length);
                    arrayOutputStream.flush();
                }//读取响应体的内容
                return arrayOutputStream.toString();//返回请求到的内容，字符串形式
            } else {
                return "-1";//如果请求失败返回-1
            }
        } catch (Exception e) {
            return "-1";//出现异常也返回-1
        }
    }

    public String get(String url1) {
        try {
            URL url = new URL(url1);
            HttpURLConnection Connection = (HttpURLConnection) url.openConnection();
            Connection.setRequestMethod("GET");
            Connection.setConnectTimeout(3000);
            Connection.setReadTimeout(3000);
            int responseCode = Connection.getResponseCode();
            if (responseCode == HttpURLConnection.HTTP_OK) {
                InputStream inputStream = Connection.getInputStream();
                ByteArrayOutputStream arrayOutputStream = new ByteArrayOutputStream();
                byte[] bytes = new byte[1024];
                int length;
                while ((length = inputStream.read(bytes)) != -1) {
                    arrayOutputStream.write(bytes, 0, length);
                    arrayOutputStream.flush();//强制释放缓冲区
                }
                return arrayOutputStream.toString();
            } else {
                return "-1";
            }
        } catch (Exception e) {
            return "-1";
        }
    }
}