def predict():
	import numpy as np
	import tensorflow as tf
	from tensorflow.keras.models import Sequential
	from tensorflow.keras.layers import LSTM, Dense
	import os
	import sys
	import glob
	from datetime import datetime
	# Устанавливаем уровень журналирования TensorFlow на ERROR
	os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'

	# Создаем обучающий набор данных, разбивая данные на последовательные блоки (999 шагов входных данных, 1 шаг выходных данных)
	def create_dataset(data, seq_length):
		X = []
		y = []
		for i in range(len(data) - seq_length):
			X.append(data[i:i + seq_length])
			y.append(data[i + seq_length])
		return np.array(X), np.array(y)

	def create_predict(data1, valname):
		data = data1[:, 3]
		if "JPY" in valname:
			data = np.array([value / 100 for value in data])
		o = data[-1]
		seq_length = 1399
		X_train, y_train = create_dataset(data, seq_length)
		X_train = X_train.reshape(X_train.shape[0], X_train.shape[1], 1)


		# model = Sequential()
		# model.reset_states()
		# # Добавление слоев LSTM
		# model.add(LSTM(units=64, return_sequences=True))
		# model.add(LSTM(units=32))

		# # Добавление полносвязных слоев
		# model.add(Dense(units=16, activation='relu'))
		# model.add(Dense(units=1))  # 1 выход для OHLC

		# # Компиляция модели
		# model.compile(optimizer='adam', loss='mean_squared_error')

		# # Обучение модели
		# model.fit(X_train, y_train, epochs=100, batch_size=64, verbose=0)
		# model.save(valname+'.keras')

		model = tf.keras.models.load_model(valname+'.keras')
		last_5_candles = data[-seq_length:].reshape(1, seq_length, 1)
		prv = model.predict(last_5_candles, verbose=0)

		# Вывод результата прогнозирования
		pov = 0.00001
		c0 = prv[:,0]
		c = c0[0]
		c = int(round((c-o),5)/pov)
		return [valname,c]


	current_date = datetime.now()
	formatted_date = current_date.strftime("%d.%m.%Y")


	file_mask = "*_D1.txt"
	file_names = glob.glob(file_mask)

	data1 = [["",0.1,0,0,0]]
	for valname in file_names:
		new_value = create_predict(np.genfromtxt(valname, delimiter=','), valname)
		data1.append(new_value)
	data1 = data1[1:]
	def max_abs(row):
	  # return max(abs(row[2][0]), abs(row[3][0]), abs(row[4][0]))
	  return row[1]

	data1.sort(key=max_abs, reverse=True) 
	result = []
	for row in data1:
	  result.append(row)
	  # if (row[2][0] < 0 and row[4][0] < 0 and row[3][0] > 0) and min(abs(row[2][0]), abs(row[3][0]), abs(row[4][0]))>25:
		# row[1][0] = "SELL 250 pips"
		# result.append(row)
	  # if (row[2][0] > 0 and row[4][0] > 0 and row[3][0] < 0) and min(abs(row[2][0]), abs(row[3][0]), abs(row[4][0]))>25:
		# row[1][0] = "BUY 250 pips"
		# result.append(row)

	# print("Prediction W1 OHLC ",formatted_date)
	file_name = "predict.log"
	temp_file_name = "temp_" + file_name
	with open(temp_file_name, "w") as file:
		print("Predict D1 ohlC ",formatted_date, file=file)
		for row in result:
		  print(row, file=file)
		print(" ", file=file)
		print(" ", file=file)
	# Читаем содержимое оригинального файла и записываем его после нового содержимого
	with open(temp_file_name, "a") as temp_file, open(file_name, "r") as original_file:
		temp_file.write(original_file.read())
	os.remove(file_name)
	os.rename(temp_file_name, file_name)
	first_element = result[0][0]
	last_element = result[-1][0]
	trimmed_first_element = first_element[:-7]
	trimmed_last_element = last_element[:-7]
	with open("predict.txt", "w") as file:
		print(trimmed_first_element, file=file)
		print(trimmed_last_element, file=file)	
	pass
# Выводим информацию о модели
# model.summary()
# predict()