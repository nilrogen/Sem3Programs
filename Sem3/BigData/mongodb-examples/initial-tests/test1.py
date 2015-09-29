import pymongo as pmg

# Set up client
client = pmg.MongoClient("localhost", 27017)

# Get Database
db = client.mydb

# create first bits of data
import datetime
post = { "author" : "Michael Gorlin",
         "title" : "Post 1",
         "text" : "This is a post idk why I did this",
         "date" : datetime.datetime.utcnow() }

posts = db.posts
post_id = posts.insert_one(post).inserted_id

#find element
print(posts.find_one({"_id" : post_id}))


