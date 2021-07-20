import csv

f = open("temp.txt", "w", encoding="utf-8")

# 開啟 CSV 檔案
with open('parsed-new.csv', newline='', encoding="utf-8") as csvfile:

  # 讀取 CSV 檔案內容
  rows = csv.reader(csvfile)

  # 以迴圈輸出每一列
  for row in rows:
    f.writelines(str(row[0]) + '\n')

f.close()
