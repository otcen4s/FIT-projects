import pyspark 
from pyspark.sql import SparkSession
from pyspark.sql.types import StructType, StringType, DoubleType, DateType
from pyspark.sql.functions import max, min, avg

local = "/media/sf_PDI/"
hdfs = "hdfs://127.0.0.2:8020/user/demo/"

spark = SparkSession.builder.master("local").appName("hdfs_test").getOrCreate()


schema = StructType() \
                    	.add("currency", StringType())\
                    	.add("time_period", DateType())\
                    	.add("obs_value", DoubleType())

# read csv from HDFS
#df1=spark.read.csv(local+"eurofxref-sdmx.csv", header=True, schema=schema)
df1=spark.read.csv(hdfs+"eurofxref-sdmx.csv", header=True, schema=schema)
df2 = df1

# get average obs_value 
_avg = df1.groupBy("currency").avg("obs_value")

# get max obs_value with a date
_max = df1.groupBy("currency").agg(max("obs_value").alias("max_row")).distinct()
cond = [df1.obs_value == _max.max_row, df1.currency == _max.currency]
res1 = df1.join(_max, cond, "leftsemi").dropDuplicates(["currency"])

# get min obs_value with a date
_min = df2.groupBy("currency").agg(min("obs_value").alias("min_row")).distinct()
cond = [df2.obs_value == _min.min_row, df2.currency == _min.currency]
res2 = df2.join(_min, cond, "leftsemi").dropDuplicates(["currency"])

# write results to csv format and save to hdfs
#res1.write.format("com.databricks.spark.csv").option("header", "true").mode("overwrite").save(local+"output_max")
#res2.write.format("com.databricks.spark.csv").option("header", "true").mode("overwrite").save(local+"output_min")
#_avg.write.format("com.databricks.spark.csv").option("header", "true").mode("overwrite").save(local+"output_avg")

res1.write.format("com.databricks.spark.csv").option("header", "true").mode("overwrite").save(hdfs+"output_max")
res2.write.format("com.databricks.spark.csv").option("header", "true").mode("overwrite").save(hdfs+"output_min")
_avg.write.format("com.databricks.spark.csv").option("header", "true").mode("overwrite").save(hdfs+"output_avg")

# get global minimal value from all obs_value
min_curr = df2.select(min("obs_value").alias("min_currency"))
cond = [df2.obs_value == min_curr.min_currency]
res3 = df2.join(min_curr, cond, "leftsemi")

# get global maximal value from all obs_value
max_curr = df1.select(max("obs_value").alias("max_currency"))
cond = [df1.obs_value == max_curr.max_currency]
res4 = df1.join(max_curr, cond, "leftsemi")

# write results to csv format and save to hdfs
#res3.write.format("com.databricks.spark.csv").option("header", "true").mode("overwrite").save(local+"global_min.csv")
#res4.write.format("com.databricks.spark.csv").option("header", "true").mode("overwrite").save(local+"global_max.csv")


res3.write.format("com.databricks.spark.csv").option("header", "true").mode("overwrite").save(hdfs+"output_global_min")
res4.write.format("com.databricks.spark.csv").option("header", "true").mode("overwrite").save(hdfs+"output_global_max")
