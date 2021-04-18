resource "aws_iam_user" "gateway" {
  name = "gateway"
}

resource "aws_iam_access_key" "gateway" {
  user = aws_iam_user.gateway.name
}