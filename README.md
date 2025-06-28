# Delivery System API
This is the delivery system backend using NodeJS, Express, and Supabase

## Features
- User authentication
- User roles (Delivery/Management)
- Delivery management
- Route Suggestions
- Delivery item suggestion


## Requirements
 - NodeJS

## Installation
1. Request `.env` file from the owner
2. Build source files
```
npm run compile
```
3. Run the program
```
npm run dev
```

## Api Endpoints
### User Routes

| Method | Endpoint         | Description          |
|--------|------------------|----------------------|
| POST   | `/api/user/register` | Register new user (management/delivery) |
| POST   | `/api/user/login`    | Login existing user |
| POST   | `/api/user/logout`   | Logout the session  |

### Delivery Routes

| Method | Endpoint                     | Description                     |
|--------|------------------------------|---------------------------------|
| GET    | `/api/delivery/all`          | Get all deliveries *(management)*    |
| GET    | `/api/delivery/`             | Get deliveries owned by session user |
| POST   | `/api/delivery/`             | Add a new delivery *(management)*    |
| PUT    | `/api/delivery/:id`          | Edit an existing delivery *(management)*       |
| DELETE | `/api/delivery/:id`          | Delete a delivery *(management)*               |
| GET    | `/api/delivery/route`        | Suggest route between source and destination *(delivery)* |
| GET    | `/api/delivery/items`        | Suggest items to deliver based on knapsack *(management)* |

## Example Usage

### Register User
```
POST /api/user/register
{
  "username": "manager1",
  "password": "secret",
  "accountType": "management"
}
```

### Add delivery
Source and destination numbers can be referenced in the database or in the `paths/city_mapping.txt` file
```
POST /api/delivery
PUT /api/delivery/${delivery_id}
{
  "product_name": "Laptop",
  "sender": "Warehouse A",
  "recipient": "Customer B",
  "source": 1,
  "destination": 5,
  "date_shipped": "2025-06-27",
  "deadline": "2025-06-29T18:00:00.000Z",
  "status": "pending"
}
```

### Delete Delivery
```
DELETE /api/delivery/${delivery_id}
```

### Suggest Delivery Route
```
GET /api/delivery/route
{
  "source": 1,
  "destination": 5
}
```

### Suggest Delivery Items
```
GET /api/delivery/items
{
  "source": 1,
  "destination": 5,
  "capacity": 100
}
```
