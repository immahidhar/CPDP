# Online Marketplace

An e-commerce platform to buy/sell products online.

## Requirements
Make sure the following are installed on the system.
* Java8
* Maven
* MySQL database

### Database tables
Run the following db scripts to create the tables needed for the software.

    CREATE DATABASE omp;

    USE omp;

    CREATE TABLE `User` (
    `id` int NOT NULL AUTO_INCREMENT,
    `username` varchar(100) NOT NULL,
    `email` varchar(100) DEFAULT NULL,
    `address` varchar(100) DEFAULT NULL,
    PRIMARY KEY (`id`),
    UNIQUE KEY `username_UNIQUE` (`username`)
    ) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=utf8mb3;

    CREATE TABLE `Product` (
    `id` int NOT NULL AUTO_INCREMENT,
    `category` varchar(45) NOT NULL,
    `name` varchar(100) DEFAULT NULL,
    `price` varchar(45) DEFAULT NULL,
    `quantity` int DEFAULT NULL,
    `address` varchar(100) DEFAULT NULL,
    PRIMARY KEY (`id`)
    ) ENGINE=InnoDB AUTO_INCREMENT=19 DEFAULT CHARSET=utf8mb3;

    CREATE TABLE `Orders` (
    `id` int NOT NULL AUTO_INCREMENT,
    `user_id` int NOT NULL,
    `product_id` int NOT NULL,
    `quantity` int NOT NULL,
    `address` varchar(100) NOT NULL,
    `status` varchar(45) NOT NULL,
    PRIMARY KEY (`id`),
    KEY `user_id_idx` (`user_id`),
    KEY `product_id_idx` (`product_id`),
    CONSTRAINT `product_id` FOREIGN KEY (`product_id`) REFERENCES `Product` (`id`),
    CONSTRAINT `user_id` FOREIGN KEY (`user_id`) REFERENCES `User` (`id`)
    ) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb3;

The Database connection can be configured by passing the following arguments at runtime:

    -Dspring.datasource.url=jdbc:mysql://localhost:3306/omp
    -Dspring.datasource.username=root
    -Dspring.datasource.password=


## Build
To build the software, enter the following:
```command line
$ mvn clean install -DskipTests
```

## Run
To run the software, execute the following:
```command line
$ java -jar -Dserver.port=8080 -Dserver.tomcat.threads.max=500 ./target/omp-1.jar
```

## API
Here are the APIs of the software.

### User
Add user

    curl --location 'http://localhost:8080/user/add' \
    --header 'Content-Type: application/json' \
    --data-raw '{
    "username": "user9",
    "email": "user@email.com",
    "address": "address and pincode"
    }'

Get all users

    curl --location 'http://localhost:8080/user/get_all'

Get user by user id or username

    curl --location 'http://localhost:8080/user/get?id=0&username=user2'

Update user

    curl --location --request PUT 'http://localhost:8080/user/update' \
    --header 'Content-Type: application/json' \
    --data-raw '{
    "id": 16,
    "username": "user99",
    "email": "user@email.com",
    "address": "address and pincode"
    }'

Delete user by user id or username

    curl --location --request DELETE 'http://localhost:8080/user/delete?id=0&username=user3'

Delete all users

    curl --location --request DELETE 'http://localhost:8080/user/delete_all'

### Product
Add product

    curl --location 'http://localhost:8080/product/add' \
    --header 'Content-Type: application/json' \
    --data '{
    "category": "books",
    "name": "House of Dragons",
    "price": 19.99,
    "quantity": 100
    }'

Get all products

    curl --location 'http://localhost:8080/product/get_all'

Get product by id, name or category

    curl --location 'http://localhost:8080/product/get?id=1&name=&category=books'

Update product

    curl --location --request PUT 'http://localhost:8080/product/update' \
    --header 'Content-Type: application/json' \
    --data '{
    "id": 7,
    "category": "books",
    "name": "Harry Potter",
    "price": 19.99,
    "quantity": 100
    }'

Delete product by id or name

    curl --location --request DELETE 'http://localhost:8080/product/delete?id=1&name=House%20of%20Dragons'

Delete all products

    curl --location --request DELETE 'http://localhost:8080/product/delete_all'

### Search
    curl --location 'http://localhost:8080/search?query=books'

### Orders
Place order

    curl --location 'http://localhost:8080/order/place' \
    --header 'Content-Type: application/json' \
    --data '{
    "userId": 8,
    "productId": 7,
    "quantity": 10,
    "address": "Student Union, FSU"
    }'

Get all orders

    curl --location 'http://localhost:8080/order/get_all'

Get order by order id or user id or product id

    curl --location 'http://localhost:8080/order/get?id=0&userId=8&productId=7'

Update order

    curl --location --request PUT 'http://localhost:8080/order/update' \
    --header 'Content-Type: application/json' \
    --data '{
    "id": 1,
    "userId": 8,
    "productId": 7,
    "quantity": 100,
    "status": "SHIPPED",
    "address": "Student Union, FSU"
    }'

Delete order by id or user id

    curl --location --request DELETE 'http://localhost:8080/order/delete?id=1&userId=8'

Delete all orders

    curl --location --request DELETE 'http://localhost:8080/order/delete_all'

