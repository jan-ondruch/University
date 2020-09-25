"""
This Python script creates a pdf based on the given title and url

"""
import os
import sys

from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import getSampleStyleSheet
from reportlab.lib.units import cm
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Image
from qrcode import *


def create_qrcode():
    """
    create_qrcode creates a qr code based on the given title
    :return:
    """
    try:
        survey_title = sys.argv[1]
        survey_url = sys.argv[2]
    except IndexError:
        print("Please check your input of the title")
        sys.exit(1)

    qr = QRCode(version=20, error_correction=ERROR_CORRECT_H)

    try:
        qr.add_data(survey_url)
    except UnboundLocalError:
        print("Please check your input of the title")
        sys.exit(1)

    qr.make()  # Generate the QRCode itself

    # im contains a PIL.Image.Image object
    im = qr.make_image()

    # To save it
    im.save(survey_title + ".png")

    return survey_title + ".png"


def create_pdf(logo_name):
    """
    create_pdf creates a pdf based on the before created qrcode as png image.
    :return:
    """

    try:
        pdf_name_with_suffix = sys.argv[1] + ".pdf"
        pdf_name = sys.argv[1]

    except IndexError:
        print("Please check your input of the title")
        sys.exit(1)

    try:
        survey_url = sys.argv[2]
    except IndexError:
        print("Please check your input of the url")
        os.remove(logo_name)
        sys.exit(1)

    created_pdf = SimpleDocTemplate(pdf_name_with_suffix, pagesize=A4,
                                    rightMargin=72, leftMargin=72,
                                    topMargin=72, bottomMargin=18)
    story = []
    logo = logo_name

    style = getSampleStyleSheet()['Normal']
    style.wordWrap = 'LTR'
    style.leading = 24
    style.fontSize = 24

    title = '<para alignment="center"><font size=24>' + pdf_name + '</font>'
    story.append(Paragraph(title, style))
    story.append(Spacer(1, 150))
    im = Image(logo, 10 * cm, 10 * cm)
    story.append(im)
    story.append(Spacer(1, 150))

    story.append(Paragraph(survey_url, style))
    created_pdf.build(story)

    os.remove(logo_name)


def main():
    """
    main method
    :return: none
    """
    logo_name = create_qrcode()
    create_pdf(logo_name)

if __name__ == "__main__":
    main()
