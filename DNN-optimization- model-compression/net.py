"""
Author: Matej Otcenas(xotcen01)
Date: 1.5.2022
Description: Module for neural net definition
"""

from tensorflow import keras

"""
Simple convnet for MNIST dataset, taken and modified from 
"""
class Net(keras.Model):
  def __init__(self, layers):
    super(Net, self).__init__()

    self._layers = layers
    # two convolution layers will be evolved
    layer_1 = layers[0] # object Layer1
    layer_2 = layers[1] # object Layer2

    self.conv1 = keras.layers.Conv2D(int(layer_1.nb_filters), 
            kernel_size=int(layer_1.filter_size), 
            activation='relu', 
            padding='same', 
            strides=(int(layer_1.stride),int(layer_1.stride)))
    self.max1 = keras.layers.MaxPooling2D(pool_size=(2, 2), padding='same')
    self.conv2 = keras.layers.Conv2D(int(layer_2.nb_filters), 
            kernel_size=int(layer_2.filter_size), 
            activation='relu', 
            padding='same', 
            strides=(int(layer_2.stride),int(layer_2.stride)))
    self.max2 = keras.layers.MaxPooling2D(pool_size=(2, 2), padding='same')
    self.flatten = keras.layers.Flatten()
    self.drop = keras.layers.Dropout(0.3)
    self.dense = keras.layers.Dense(10, activation="softmax")

  # self calling function for forward pass
  def call(self, inputs):
      x = self.conv1(inputs)
      x = self.max1(x)
      x = self.conv2(x)
      x = self.max2(x)
      x = self.flatten(x)
      x = self.drop(x)
      return self.dense(x)
  # redefined summary function for printing to txt file
  def summary(self, report_file):
      x = keras.Input(shape=(28, 28, 1))
      model = keras.Model(inputs=[x], outputs=self.call(x))
      return model.summary(print_fn=lambda x: report_file.write(x + "\n"))