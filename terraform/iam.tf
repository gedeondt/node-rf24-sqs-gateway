resource "aws_iam_user" "gateway" {
  name = "gateway"
}

resource "aws_iam_access_key" "gateway" {
  user = aws_iam_user.gateway.name
}

output "gateway_key_id" {
  value = aws_iam_access_key.gateway.id
}

output "gateway_key_key" {
  value = aws_iam_access_key.gateway.secret
  sensitive   = true
}
