resource "aws_lambda_function" "credentials" {
   function_name = "credentials"
   filename      = "lambda/credentials.js.zip"

    environment {
        variables = {
         ROLE_ARN = aws_iam_role.domotics_role.arn,
         PASSWORDS = "{\"test\":\"123456789\"}"
        }
    }
   
   handler = "credentials.handler"
   runtime = "nodejs12.x"

   role = aws_iam_role.credentials_lambda_role.arn
}

resource "aws_iam_role" "credentials_lambda_role" {
   name = "credentials_lambda_role"
   assume_role_policy = <<EOF
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Action": "sts:AssumeRole",
      "Principal": {
        "Service": "lambda.amazonaws.com"
      },
      "Effect": "Allow",
      "Sid": ""
    }
  ]
}
EOF
}

module "apigateway_with_cors" {
  source  = "./modules/api-cors"

  lambda_function_name = aws_lambda_function.credentials.function_name
  lambda_invoke_arn    = aws_lambda_function.credentials.invoke_arn
  http_method = "POST"
  path_part = "credentials"
}

resource "aws_api_gateway_domain_name" "credentials_domain" {
  count = var.credentials_domain != "" ? 1 : 0

  certificate_arn = var.credentials_cert_arn
  domain_name     = "${var.credentials_subdomain}.${var.credentials_domain}"
}

resource "aws_route53_record" "credentials_domain_zone_record" {
  count   = var.credentials_domain_zone_id != "" ? 1 : 0
  name    = var.credentials_subdomain
  type    = "A"
  zone_id = var.credentials_domain_zone_id

  alias {
    evaluate_target_health = true
    name       = aws_api_gateway_domain_name.credentials_domain[0].cloudfront_domain_name
    zone_id    = aws_api_gateway_domain_name.credentials_domain[0].cloudfront_zone_id
  }
}

output "base_url" {
  value = module.apigateway_with_cors.lambda_url
}

