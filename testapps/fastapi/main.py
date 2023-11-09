from fastapi import FastAPI, Request, Response, Depends, HTTPException
from starlette.middleware import Middleware
from pinpointPy.Fastapi import PinPointMiddleWare, async_monkey_patch_for_pinpoint
from pinpointPy import set_agent
from sqlalchemy.orm import Session
from starlette_context.middleware import ContextMiddleware
import aioredis
from contextlib import asynccontextmanager
import httpx
from typing import List

import crud
import models
import schemas

from database import SessionLocal, engine

models.Base.metadata.create_all(bind=engine)

redis = aioredis.from_url('redis://redis:6379', decode_responses=True)


class UserMiddleWare(PinPointMiddleWare):
    async def dispatch(self, request: Request, call_next):
        if request.url.path in ["/heartbeat", "/l7check"]:
            return await call_next(request)
        else:
            return await super().dispatch(request, call_next)


middleware = [
    Middleware(ContextMiddleware),
    Middleware(UserMiddleWare)
]

async_monkey_patch_for_pinpoint()

set_agent("cd.dev.test.py", "cd.dev.test.py", 'tcp:dev-collector:10000')


@asynccontextmanager
async def lifespan(app: FastAPI):
    app.requests_client = httpx.AsyncClient()
    yield
    await app.requests_client.aclose()

app = FastAPI(title='FastAPI Pinpoint Example',
              middleware=middleware, lifespan=lifespan)


def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()


@app.middleware("http")
async def db_session_middleware(request: Request, call_next):
    response = Response("Internal server error", status_code=500)
    try:
        request.state.db = SessionLocal()
        response = await call_next(request)
    finally:
        request.state.db.close()
    return response


def get_db(request: Request):
    return request.state.db


@app.get("/")
async def root():
    return {"message": "Hello World"}


@app.get("/test-redis/set/{uid}", tags=["redis"])
async def test_redis(uid: str = "default"):
    await redis.set(uid, "50fdf310-7d3b-11ee-b962-0242ac120002", ex=1)
    in_value = await redis.get(uid)
    return {"uid": in_value}


@app.get("/httpx/example", tags=["httpx"])
async def test_httpx(request: Request, url='http://www.example.com/'):
    requests_client = request.app.requests_client
    print(request.headers)
    response = await requests_client.get(url)
    return {"response": response.status_code}


@app.get("/httpx/backend", tags=["httpx"])
async def test_httpx_backend(request: Request, url='http://backend:8000/'):
    requests_client = request.app.requests_client
    print(request.headers)
    response = await requests_client.get(url)
    return {"response": response.status_code}


@app.get("/httpx-self/", tags=["httpx"])
async def test_httpx(request: Request):
    requests_client = request.app.requests_client
    response = await requests_client.get('http://127.0.0.1:8000/httpx/example')
    return {"response": response.status_code}

# thanks guide from https://fastapi.tiangolo.com/tutorial/sql-databases/


@app.post("/users/", response_model=schemas.User, tags=["mysql"])
def create_user(user: schemas.UserCreate, db: Session = Depends(get_db)):
    db_user = crud.get_user_by_email(db, email=user.email)
    if db_user:
        raise HTTPException(status_code=400, detail="Email already registered")
    return crud.create_user(db=db, user=user)


@app.get("/users/", response_model=List[schemas.User], tags=["mysql"])
def read_users(skip: int = 0, limit: int = 100, db: Session = Depends(get_db)):
    users = crud.get_users(db, skip=skip, limit=limit)
    return users


@app.get("/users/{user_id}", response_model=schemas.User, tags=["mysql"])
def read_user(user_id: int, db: Session = Depends(get_db)):
    db_user = crud.get_user(db, user_id=user_id)
    if db_user is None:
        raise HTTPException(status_code=404, detail="User not found")
    return db_user


@app.post("/users/{user_id}/items/", response_model=schemas.Item, tags=["mysql"])
def create_item_for_user(
    user_id: int, item: schemas.ItemCreate, db: Session = Depends(get_db)
):
    return crud.create_user_item(db=db, item=item, user_id=user_id)


@app.get("/items/", response_model=List[schemas.Item], tags=["mysql"])
def read_items(skip: int = 0, limit: int = 100, db: Session = Depends(get_db)):
    items = crud.get_items(db, skip=skip, limit=limit)
    return items
