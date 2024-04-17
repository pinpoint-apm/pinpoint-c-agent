from django.urls import path

from . import views

urlpatterns = [
    path("", views.index, name="index"),
    path('products/<int:pk>/', views.get_products, name="index"),
    path('call/', views.get_url,name="remote"),
    path('call_postgres/',views.get_postgres,name="postgres")
]
