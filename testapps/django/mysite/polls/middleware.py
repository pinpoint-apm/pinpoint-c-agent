from pinpointPy.Django import DjangoMiddleWare


def UserMiddleWare(get_response):

    pinpoint_handle_request = DjangoMiddleWare(get_response)

    def user_request_route(request):
        if request.path in ['/heart_check']:
            response = get_response(request)
        else:
            response = pinpoint_handle_request(request)
        return response

    return user_request_route
