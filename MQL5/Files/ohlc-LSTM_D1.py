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
	seq_length = 1399
	opt = 0

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
		X_train, y_train = create_dataset(data, seq_length)
		X_train = X_train.reshape(X_train.shape[0], X_train.shape[1], 1)
		if opt == 1:
			model = Sequential()
			model.reset_states()
			model.add(LSTM(units=64, return_sequences=True))
			model.add(LSTM(units=32))
			model.add(Dense(units=16, activation='relu'))
			model.add(Dense(units=1))  # 1 выход для OHLC
			model.compile(optimizer='adam', loss='mean_squared_error')
			model.fit(X_train, y_train, epochs=100, batch_size=64, verbose=0)
			model.save(valname+'_Close.keras')
		else:
			model = tf.keras.models.load_model(valname+'_Close.keras')
		last_5_candles = data[-seq_length:].reshape(1, seq_length, 1)
		prv = model.predict(last_5_candles, verbose=0)
		pov = 0.00001
		c0 = prv[:,0]
		c = c0[0]
		c = int(round((c-o),5)/pov)
		
		data = data1[:, 2]
		if "JPY" in valname:
			data = np.array([value / 100 for value in data])
		o = data[-1]		
		X_train, y_train = create_dataset(data, seq_length)
		X_train = X_train.reshape(X_train.shape[0], X_train.shape[1], 1)
		if opt == 1:
			model = Sequential()
			model.reset_states()
			model.add(LSTM(units=64, return_sequences=True))
			model.add(LSTM(units=32))
			model.add(Dense(units=16, activation='relu'))
			model.add(Dense(units=1))  # 1 выход для OHLC
			model.compile(optimizer='adam', loss='mean_squared_error')
			model.fit(X_train, y_train, epochs=100, batch_size=64, verbose=0)
			model.save(valname+'_Low.keras')
		else:
			model = tf.keras.models.load_model(valname+'_Low.keras')
		last_5_candles = data[-seq_length:].reshape(1, seq_length, 1)
		prv = model.predict(last_5_candles, verbose=0)
		pov = 0.00001
		l0 = prv[:,0]
		l = l0[0]
		l = int(round((l-o),5)/pov)		
		
		data = data1[:, 1]
		if "JPY" in valname:
			data = np.array([value / 100 for value in data])
		o = data[-1]		
		X_train, y_train = create_dataset(data, seq_length)
		X_train = X_train.reshape(X_train.shape[0], X_train.shape[1], 1)
		if opt == 1:
			model = Sequential()
			model.reset_states()
			model.add(LSTM(units=64, return_sequences=True))
			model.add(LSTM(units=32))
			model.add(Dense(units=16, activation='relu'))
			model.add(Dense(units=1))  # 1 выход для OHLC
			model.compile(optimizer='adam', loss='mean_squared_error')
			model.fit(X_train, y_train, epochs=100, batch_size=64, verbose=0)
			model.save(valname+'_High.keras')
		else:
			model = tf.keras.models.load_model(valname+'_High.keras')
		last_5_candles = data[-seq_length:].reshape(1, seq_length, 1)
		prv = model.predict(last_5_candles, verbose=0)
		pov = 0.00001
		h0 = prv[:,0]
		h = h0[0]
		h = int(round((h-o),5)/pov)			
		return [valname,c,h,l]


	current_date = datetime.now()
	formatted_date = current_date.strftime("%d.%m.%Y")


	file_mask = "*_D1.txt"
	file_names = glob.glob(file_mask)

	data1 = [["",0.1,0,0,0]]
	for valname in file_names:
		new_value = create_predict(np.genfromtxt(valname, delimiter=','), valname)
		data1.append(new_value)
	data1 = data1[1:]
	
	result = []
	for row in data1:
		valname, c, h, l = row
		result.append([
			valname, 
			int(c*0.4 + h*0.3 + l*0.3)
		])	
		
	def Avg(row):
	  # return max(abs(row[2][0]), abs(row[3][0]), abs(row[4][0]))
	  return row[1]	
	def Close(row):
	  # return max(abs(row[2][0]), abs(row[3][0]), abs(row[4][0]))
	  return row[1]
	def Low(row):
	  # return max(abs(row[2][0]), abs(row[3][0]), abs(row[4][0]))
	  return row[3]
	def High(row):
	  # return max(abs(row[2][0]), abs(row[3][0]), abs(row[4][0]))
	  return row[2]

	result.sort(key=Avg, reverse=True) 
	resultA = []
	for row in result:
	  resultA.append([row[0],row[1]])
	  
	data1.sort(key=Close, reverse=True) 
	resultC = []
	for row in data1:
	  resultC.append([row[0],row[1]])
	  # if (row[2][0] < 0 and row[4][0] < 0 and row[3][0] > 0) and min(abs(row[2][0]), abs(row[3][0]), abs(row[4][0]))>25:
		# row[1][0] = "SELL 250 pips"
		# result.append(row)
	  # if (row[2][0] > 0 and row[4][0] > 0 and row[3][0] < 0) and min(abs(row[2][0]), abs(row[3][0]), abs(row[4][0]))>25:
		# row[1][0] = "BUY 250 pips"
		# result.append(row)
		
	data1.sort(key=Low, reverse=True) 
	resultL = []
	for row in data1:
	  resultL.append([row[0],row[3]])

	data1.sort(key=High, reverse=True) 
	resultH = []
	for row in data1:
	  resultH.append([row[0],row[2]])
	  
	# print("Prediction W1 OHLC ",formatted_date)
	file_name = "predict.log"
	temp_file_name = "temp_" + file_name
	with open(temp_file_name, "w") as file:
		print("Predict D1 oHLC ",formatted_date, file=file)
		# for row in result:
		  # print(row, file=file)
		for row1, row2, row3, row4 in zip(resultC, resultH, resultL, resultA):
			print(row1, " ", row2, " ", row3, " ", row4, file=file)
		print(" ", file=file)
		print(" ", file=file)
	# Читаем содержимое оригинального файла и записываем его после нового содержимого
	with open(temp_file_name, "a") as temp_file, open(file_name, "r") as original_file:
		temp_file.write(original_file.read())
	os.remove(file_name)
	os.rename(temp_file_name, file_name)
	first_element = resultA[0][0]
	last_element = resultA[-1][0]
	trimmed_first_element = first_element[:-7]
	trimmed_last_element = last_element[:-7]
	with open("predict.txt", "w") as file:
		print(trimmed_first_element, file=file)
		print(trimmed_last_element, file=file)	
	pass
# Выводим информацию о модели
# model.summary()
# predict()