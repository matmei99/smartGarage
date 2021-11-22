import distutils.util
import json

from status import Status
from flask import Flask
from flask_restful import Resource, Api, reqparse

mystatus = Status()

app = Flask(__name__)
api = Api(app)


class GarageStatus(Resource):
    def get(self):
        return mystatus.to_json()

    def post(self):
        parser = reqparse.RequestParser()
        parser.add_argument('trigger', required=True)
        args = parser.parse_args()

        boolString = args['trigger']
        mystatus.openGarage = bool(distutils.util.strtobool(boolString))
        return mystatus.to_json()

    pass


api.add_resource(GarageStatus, '/status')

app.run()  # run our Flask app

print(mystatus.openGarage)
