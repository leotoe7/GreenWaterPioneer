void qianjin() {
  pinMode(D5, OUTPUT);
  digitalWrite(D5, 1);
  pinMode(D7, INPUT);
  //server.send(200, "text/html", "<h1>qianjin</h1>");
}
void houtui() {
  pinMode(D5, OUTPUT);
  digitalWrite(D5, 0);
  pinMode(D7, INPUT);
  //server.send(200, "text/html", "<h1>houtui</h1>");
}

void zuozhuan() {
  pinMode(D7, OUTPUT);
  digitalWrite(D7, 1);
  pinMode(D5, INPUT);
//  server.send(200, "text/html", "<h1>zuozhuan</h1>");
}

void youzhuan() {
  pinMode(D7, OUTPUT);
  digitalWrite(D7, 0);
  pinMode(D5, INPUT);
//  server.send(200, "text/html", "<h1>youzhuan</h1>");
}

void htzuozhuan() {
  pinMode(D5, OUTPUT);
  digitalWrite(D5, 0);
  pinMode(D7, OUTPUT);
  digitalWrite(D7, 1);
//  pinMode(D5, INPUT);
//  server.send(200, "text/html", "<h1>zuozhuan</h1>");
}

void htyouzhuan() {
  pinMode(D5, OUTPUT);
  digitalWrite(D5, 0);
  pinMode(D7, OUTPUT);
  digitalWrite(D7, 0);
//  pinMode(D5, INPUT);
//  server.send(200, "text/html", "<h1>youzhuan</h1>");
}

void tingzhi1() {
  pinMode(D5, INPUT);
//  server.send(200, "text/html", "<h1>tingzhi1</h1>");
}

void tingzhi2() {
  pinMode(D7, INPUT);
//  server.send(200, "text/html", "<h1>tingzhi2</h1>");
}
